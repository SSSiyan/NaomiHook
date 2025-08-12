#include "ShaderEdit.hpp"
#include <d3dcompiler.h>

const ModToggle::Ptr g_mod_enabled{ModToggle::create("shader_edit_swap_shader_toggle", false)};
const ModToggle::Ptr g_gamma_enabled{ModToggle::create("shader_edit_gamma_toggle", false)}; // gamma correction toggle

static float g_shader_contrast[3]  = {1.09f, 1.09f, 1.09f};
static float g_shader_saturation   = 1.0f;
static float g_shadow_darkening    = 1.0f;
static float g_midtone_darkening   = 1.0f;
static float g_highlight_darkening = 1.0f;

static float g_gamma_amount = 1.0f; // 0.10..3.00

const char* shader_fmt = R"(
// ---- Custom Shader with Contrast, Saturation, Shadows, Midtones, Highlights (+ optional Gamma)

SamplerState BaseTextureSampler_s : register(s0);
Texture2D<float4> BaseTexture : register(t0);

// Host-injected constants
static const float3 kContrast    = float3(%f, %f, %f);
static const float  kSaturation  = %f;
static const float  kShadow      = %f;
static const float  kMidtone     = %f;
static const float  kHighlight   = %f;

// Gamma controls
static const float  kGammaOn     = %f; // 0 or 1
static const float  kGamma       = %f; // 0.1..3.0 (1.0 = no change)

// --- utilities ---
float luminance(float3 color) {
    // 601 weights
    return dot(color, float3(0.299, 0.587, 0.114));
}
float3 adjustLevels(float3 color, float shadow, float midtone, float highlight) {
    float lum = luminance(color);
    float3 result = color;

    if (lum < 0.333) {
        float factor = smoothstep(0.0, 0.333, lum);
        factor = lerp(shadow, 1.0, factor);
        result *= factor;
    } else if (lum < 0.666) {
        float factor = smoothstep(0.333, 0.666, lum);
        factor = lerp(midtone, 1.0, factor);
        result *= factor;
    } else {
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
    float2 uv = v2.xy;
    float4 col = BaseTexture.Sample(BaseTextureSampler_s, uv);

    // Per-channel contrast
    col.rgb = (col.rgb - 0.5) * kContrast + 0.5;

    // Saturation
    float gray = luminance(col.rgb);
    col.rgb = lerp(float3(gray, gray, gray), col.rgb, kSaturation);

    // Shadows / Midtones / Highlights
    col.rgb = adjustLevels(col.rgb, kShadow, kMidtone, kHighlight);

    // Gamma (apply in grading chain)
    if (kGammaOn > 0.5) {
        float g = max(kGamma, 1e-3);
        col.rgb = pow(saturate(col.rgb), 1.0 / g);
    }

    o0 = v0 * col;
    return;
}
)";

class EEShaderResourceSomething {
public:
    char pad_0000[28];
    ID3D11PixelShader* d3d11_ps;
    char pad_0020[4];
    class ShaderBytecode* shader_bytecode;
    uint32_t size_maybe;
};

class ShaderBytecode {
public:
    char pad_0000[596];
    glm::vec3 contrast;
    char pad_0260[3492];
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

// original game shader with broken cc
ID3D11PixelShader* g_game_ps_original = nullptr;
// keep track of our edited shader for freeing later
ID3D11PixelShader* g_our_ps_edited = nullptr;

static int recreate_shader(
    float* contrast, float saturation, float shadow, float midtone, float highlight, bool gamma_on, float gamma_amount) {
    EEShaderResourceSomething* ours = g_our_edited_shader_ptr;
    if (!ours)
        return -1;

    ID3D11PixelShader* new_ps{nullptr};

    static bool initialized = [](EEShaderResourceSomething* eesr) {
        g_game_ps_original = eesr->d3d11_ps;
        return true;
    }(ours);

    if (!g_mod_enabled->value()) {
        if (ours->d3d11_ps == g_game_ps_original)
            return 1;
        ID3D11PixelShader** game_ps_loc = &(ours->d3d11_ps);
        InterlockedCompareExchange((LONG*)game_ps_loc, (LONG)(g_game_ps_original), (LONG)(ours->d3d11_ps));
        return 1;
    }

    auto device = g_framework->d3d11()->get_device();

    char buffer[16384] = {0};
    sprintf(buffer, shader_fmt, contrast[0], contrast[1], contrast[2], saturation, shadow, midtone, highlight, gamma_on ? 1.0f : 0.0f,
        gamma_amount);

    if (pErrorBlob) {
        pErrorBlob->Release();
        pErrorBlob = nullptr;
    }
    if (pBlob) {
        pBlob->Release();
        pBlob = nullptr;
    }

    HRESULT hr = D3DCompile(buffer, (UINT)strlen(buffer), nullptr, nullptr, nullptr, "main", "ps_5_0", 0, 0, &pBlob, &pErrorBlob);
    if (FAILED(hr)) {
        if (pErrorBlob) {
            OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
            pErrorBlob->Release();
            pErrorBlob = nullptr;
        }
        return -1;
    }

    if (FAILED(device->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &new_ps))) {
        if (pBlob) {
            pBlob->Release();
            pBlob = nullptr;
        }
        return -1;
    }

    if (ours->d3d11_ps == g_our_ps_edited && ours->d3d11_ps) {
        ours->d3d11_ps->Release();
        g_our_ps_edited = new_ps;
    } else {
        g_our_ps_edited = new_ps;
    }

    ID3D11PixelShader** game_ps_loc = &(ours->d3d11_ps);
    InterlockedCompareExchange((LONG*)game_ps_loc, (LONG)new_ps, (LONG)(ours->d3d11_ps));

    if (pBlob) {
        pBlob->Release();
        pBlob = nullptr;
    }
    if (pErrorBlob) {
        pErrorBlob->Release();
        pErrorBlob = nullptr;
    }

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

    g_gamma_amount = cfg.get<float>("shaderGammaAmount").value_or(1.0f);

    g_mod_enabled->config_load(cfg);
    g_gamma_enabled->config_load(cfg);

    if (g_mod_enabled->value()) {
        recreate_shader(g_shader_contrast, g_shader_saturation, g_shadow_darkening, g_midtone_darkening, g_highlight_darkening,
            g_gamma_enabled->value(), g_gamma_amount);
    }
}

void ShaderEdit::on_config_save(utility::Config& cfg) {
    cfg.set<float>("shaderContrastR", g_shader_contrast[0]);
    cfg.set<float>("shaderContrastG", g_shader_contrast[1]);
    cfg.set<float>("shaderContrastB", g_shader_contrast[2]);
    cfg.set<float>("shaderSaturation", g_shader_saturation);
    cfg.set<float>("shadowDarkening", g_shadow_darkening);
    cfg.set<float>("midtoneDarkening", g_midtone_darkening);
    cfg.set<float>("highlightDarkening", g_highlight_darkening);

    cfg.set<float>("shaderGammaAmount", g_gamma_amount);

    g_mod_enabled->config_save(cfg);
    g_gamma_enabled->config_save(cfg);
}

void ShaderEdit::on_draw_ui() {
    if (g_mod_enabled->draw("Enable shader replacement?")) {
        recreate_shader(g_shader_contrast, g_shader_saturation, g_shadow_darkening, g_midtone_darkening, g_highlight_darkening,
            g_gamma_enabled->value(), g_gamma_amount);
    }
    if (!g_mod_enabled->value())
        return;

    int shader_created = 0;

    float average_contrast           = (g_shader_contrast[0] + g_shader_contrast[1] + g_shader_contrast[2]) / 3.0f;
    static bool use_uniform_contrast = true;

    ImGui::Checkbox("Use uniform contrast", &use_uniform_contrast);

    bool need_recreate = false;

    if (use_uniform_contrast) {
        if (ImGui::SliderFloat("Adjust contrast", &average_contrast, 0.0f, 3.0f, "%.2f")) {
            g_shader_contrast[0] = g_shader_contrast[1] = g_shader_contrast[2] = average_contrast;
            need_recreate                                                      = true;
        }
    } else {
        if (ImGui::DragFloat3("Adjust contrast (RGB)", g_shader_contrast, 0.1f, 0.0f, 3.0f)) {
            need_recreate = true;
        }
    }

    if (ImGui::SliderFloat("Saturation", &g_shader_saturation, 0.0f, 3.0f, "%.2f")) {
        need_recreate = true;
    }
    if (ImGui::SliderFloat("Shadow Darkening", &g_shadow_darkening, 0.0f, 2.0f, "%.2f")) {
        need_recreate = true;
    }

    // Gamma
    if (g_gamma_enabled->draw("Gamma Correction"))
        need_recreate = true;
    if (g_gamma_enabled->value()) {
        need_recreate |= ImGui::SliderFloat("Gamma", &g_gamma_amount, 0.10f, 3.00f, "%.2f");
    }

    if (need_recreate) {
        shader_created = recreate_shader(g_shader_contrast, g_shader_saturation, g_shadow_darkening, g_midtone_darkening,
            g_highlight_darkening, g_gamma_enabled->value(), g_gamma_amount);
    }

    if (shader_created == 1) {
        ImGui::TextColored(ImColor(0.3f, 1.0f, 0.3f, 1.0f), "Shader created!");
    }
    if (shader_created == -1) {
        ImGui::TextColored(ImColor(1.0f, 0.3f, 0.3f, 1.0f), "Failed to create a shader!");
    }
}
