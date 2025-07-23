#include "ShaderEdit.hpp"
#include <d3dcompiler.h>

static float g_shader_contrast[3]  = {1.09f, 1.09f, 1.09f};
static float g_shader_saturation   = 1.0f;
static float g_shadow_darkening    = 1.0f;
static float g_midtone_darkening   = 1.0f;
static float g_highlight_darkening = 1.0f;

const char* shader_fmt = R"(
// ---- Custom Shader with Contrast, Saturation, Shadows, Midtones, Highlights

SamplerState BaseTextureSampler_s : register(s0);
Texture2D<float4> BaseTexture : register(t0);

float luminance(float3 color) {
    return dot(color, float3(0.299, 0.587, 0.114));
}

float3 adjustLevels(float3 color, float shadow, float midtone, float highlight) {
    float lum = luminance(color);
    float3 result = color;

    // Shadows
    if (lum < 0.333) {
        float factor = smoothstep(0.0, 0.333, lum);
        factor = lerp(shadow, 1.0, factor);
        result *= factor;
    }
    // Midtones
    else if (lum < 0.666) {
        float factor = smoothstep(0.333, 0.666, lum);
        factor = lerp(midtone, 1.0, factor);
        result *= factor;
    }
    // Highlights
    else {
        float factor = smoothstep(0.666, 1.0, lum);
        factor = lerp(highlight, 1.0, factor);
        result *= factor;
    }
    return result;
}

void main(
    float4 v0 : COLOR0,
    float4 v1 : SV_POSITION0,
    float2 v2 : TEXCOORD0,
    out float4 o0 : SV_TARGET0)
{
    float4 col = BaseTexture.Sample(BaseTextureSampler_s, v2.xy);

    // Apply per-channel contrast
    float3 contrast = float3(%f, %f, %f);
    col.rgb = (col.rgb - 0.5) * contrast + 0.5;

    // Apply saturation
    float gray = luminance(col.rgb);
    col.rgb = lerp(float3(gray, gray, gray), col.rgb, %f);

    // Apply shadows, midtones, highlights darkening
    col.rgb = adjustLevels(col.rgb, %f, %f, %f);

    o0 = v0 * col;
    return;
}
)";

class EEShaderResourceSomething {
public:
    char pad_0000[28];                     // 0x0000
    ID3D11PixelShader* d3d11_ps;           // 0x001C
    char pad_0020[4];                      // 0x0020
    class ShaderBytecode* shader_bytecode; // 0x0024
    uint32_t size_maybe;                   // 0x0028
};

class ShaderBytecode {
public:
    char pad_0000[596];  // 0x0000
    glm::vec3 contrast;  // 0x0254
    char pad_0260[3492]; // 0x0260
};

EEShaderResourceSomething* g_our_edited_shader_ptr{nullptr};

void find_our_edited_shader(EEShaderResourceSomething* ee) {
    if (!g_our_edited_shader_ptr) {
        if (ee->size_maybe == 0x334 || ee->size_maybe == 0x2F0) {
            g_our_edited_shader_ptr = ee;
        }
    }
}

static uintptr_t set_rt_hook_jmp_back{NULL};
naked void set_rt_detour() {
    __asm {
        mov eax, [eax + edi * 4 + 0B4h]
        cmp DWORD PTR [g_our_edited_shader_ptr], 0
        jne shaderFound
        pushad
        push eax
        call find_our_edited_shader
        pop eax
        popad
    shaderFound:
        jmp qword ptr [set_rt_hook_jmp_back]
    }
}

ID3DBlob* pBlob      = nullptr;
ID3DBlob* pErrorBlob = nullptr;
static int recreate_shader(float* contrast, float saturation, float shadow, float midtone, float highlight) {
    EEShaderResourceSomething* ours = g_our_edited_shader_ptr;
    ID3D11PixelShader* new_ps{nullptr};

    auto device = g_framework->d3d11()->get_device();

    char buffer[2048] = {0};
    sprintf(buffer, shader_fmt, contrast[0], contrast[1], contrast[2], saturation, shadow, midtone, highlight);

    HRESULT hr = D3DCompile(buffer, strlen(buffer), nullptr, nullptr, nullptr, "main", "ps_5_0", 0, 0, &pBlob, &pErrorBlob);
    if (FAILED(hr)) {
        if (pErrorBlob) {
            OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
            pErrorBlob->Release();
        }
        return -1;
    }

    if (FAILED(device->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &new_ps))) {
        return -1;
    }

    ours->d3d11_ps->Release();
    ID3D11PixelShader** game_ps_loc = &(ours->d3d11_ps);
    InterlockedCompareExchange((LONG*)game_ps_loc, (LONG)new_ps, (LONG)(ours->d3d11_ps));
    return 1;
}

std::optional<std::string> ShaderEdit::on_initialize() {
    static bool initialized = false;
    if (initialized) {
        return Mod::on_initialize();
    }

    if (!install_hook_offset(0x29852, m_hook, set_rt_detour, &set_rt_hook_jmp_back, 7)) {
        return "Failed to install shader edits hook";
    }

    initialized = true;
    return Mod::on_initialize();
}

void ShaderEdit::on_config_load(const utility::Config& cfg) {
    g_shader_contrast[0]  = cfg.get<float>("shaderContrastR").value_or(1.09f);
    g_shader_contrast[1]  = cfg.get<float>("shaderContrastG").value_or(1.09f);
    g_shader_contrast[2]  = cfg.get<float>("shaderContrastB").value_or(1.09f);
    g_shader_saturation   = cfg.get<float>("shaderSaturation").value_or(1.0f);
    g_shadow_darkening    = cfg.get<float>("shadowDarkening").value_or(1.0f);
    g_midtone_darkening   = cfg.get<float>("midtoneDarkening").value_or(1.0f);
    g_highlight_darkening = cfg.get<float>("highlightDarkening").value_or(1.0f);

    recreate_shader(g_shader_contrast, g_shader_saturation, g_shadow_darkening, g_midtone_darkening, g_highlight_darkening);
}

void ShaderEdit::on_config_save(utility::Config& cfg) {
    cfg.set<float>("shaderContrastR", g_shader_contrast[0]);
    cfg.set<float>("shaderContrastG", g_shader_contrast[1]);
    cfg.set<float>("shaderContrastB", g_shader_contrast[2]);
    cfg.set<float>("shaderSaturation", g_shader_saturation);
    cfg.set<float>("shadowDarkening", g_shadow_darkening);
    cfg.set<float>("midtoneDarkening", g_midtone_darkening);
    cfg.set<float>("highlightDarkening", g_highlight_darkening);
}

void ShaderEdit::on_draw_ui() {
    int shader_created = 0;

    float average_contrast           = (g_shader_contrast[0] + g_shader_contrast[1] + g_shader_contrast[2]) / 3.0f;
    static bool use_uniform_contrast = true;

    ImGui::Checkbox("Use uniform contrast", &use_uniform_contrast);

    if (use_uniform_contrast) {
        if (ImGui::SliderFloat("Adjust contrast", &average_contrast, 0.0f, 3.0f, "%.2f")) {
            g_shader_contrast[0] = average_contrast;
            g_shader_contrast[1] = average_contrast;
            g_shader_contrast[2] = average_contrast;
            shader_created =
                recreate_shader(g_shader_contrast, g_shader_saturation, g_shadow_darkening, g_midtone_darkening, g_highlight_darkening);
        }
    } else {
        if (ImGui::DragFloat3("Adjust contrast (RGB)", g_shader_contrast, 0.1f, 0.0f, 3.0f)) {
            shader_created =
                recreate_shader(g_shader_contrast, g_shader_saturation, g_shadow_darkening, g_midtone_darkening, g_highlight_darkening);
        }
    }

    if (ImGui::SliderFloat("Saturation", &g_shader_saturation, 0.0f, 3.0f, "%.2f")) {
        shader_created =
            recreate_shader(g_shader_contrast, g_shader_saturation, g_shadow_darkening, g_midtone_darkening, g_highlight_darkening);
    }
    if (ImGui::SliderFloat("Shadow Darkening", &g_shadow_darkening, 0.0f, 2.0f, "%.2f")) {
        shader_created =
            recreate_shader(g_shader_contrast, g_shader_saturation, g_shadow_darkening, g_midtone_darkening, g_highlight_darkening);
    }
    if (ImGui::SliderFloat("Midtone Darkening", &g_midtone_darkening, 0.0f, 2.0f, "%.2f")) {
        shader_created =
            recreate_shader(g_shader_contrast, g_shader_saturation, g_shadow_darkening, g_midtone_darkening, g_highlight_darkening);
    }
    if (ImGui::SliderFloat("Highlight Darkening", &g_highlight_darkening, 0.0f, 2.0f, "%.2f")) {
        shader_created =
            recreate_shader(g_shader_contrast, g_shader_saturation, g_shadow_darkening, g_midtone_darkening, g_highlight_darkening);
    }

    if (shader_created == 1) {
        ImGui::TextColored(ImColor(0.3f, 1.0f, 0.3f, 1.0f), "Shader created!");
    }
    if (shader_created == -1) {
        ImGui::TextColored(ImColor(1.0f, 0.3f, 0.3f, 1.0f), "Failed to create a shader!");
    }
}
