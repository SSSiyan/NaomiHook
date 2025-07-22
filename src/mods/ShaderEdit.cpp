#include "ShaderEdit.hpp"
#include <d3dcompiler.h>

static float g_shader_contrast[3] = {1.09f, 1.09f, 1.09f};
const char* shader_fmt = R"(
// ---- Created with 3Dmigoto v1.3.16 on Sat Jul 12 22:42:42 2025

SamplerState BaseTextureSampler_s : register(s0);
Texture2D<float4> BaseTexture : register(t0);


// 3Dmigoto declarations
#define cmp -

void main(
  float4 v0 : COLOR0,
  float4 v1 : SV_POSITION0,
  float2 v2 : TEXCOORD0,
  out float4 o0 : SV_TARGET0)
{
  float4 r0;
  uint4 bitmask, uiDest;
  float4 fDest;

  r0.xyzw = BaseTexture.Sample(BaseTextureSampler_s, v2.xy).xyzw;
    float4 col = r0;
    float3 fac = float3(%f, %f, %f);
    col.rgb = (col.rgb - 0.50f) * fac.rgb + 0.50f;
    o0.xyzw = v0.xyzw * col.xyzw; //r0.xyzw;
  return;
}
)";

class EEShaderResourceSomething
{
public:
    char pad_0000[28]; //0x0000
    ID3D11PixelShader* d3d11_ps; //0x001C
    char pad_0020[4]; //0x0020
    class ShaderBytecode *shader_bytecode; //0x0024
    uint32_t size_maybe; //0x0028
}; //Size: 0x002C

class ShaderBytecode
{
public:
    char pad_0000[596]; //0x0000
    glm::vec3 contrast; //0x0254
    char pad_0260[3492]; //0x0260
}; //Size: 0x1004

EEShaderResourceSomething* g_our_edited_shader_ptr {nullptr};

void find_our_edited_shader(EEShaderResourceSomething* ee) {
    if (!g_our_edited_shader_ptr) {
        // our shader edit
        if (ee->size_maybe == 0x334) {
            g_our_edited_shader_ptr = ee;
        }
        // default color_tex0.ps
        if (ee->size_maybe == 0x2F0) {
            g_our_edited_shader_ptr = ee;
        }
    }
}

// clang-format off
static uintptr_t set_rt_hook_jmp_back {NULL};
naked void set_rt_detour() {
    __asm {
        mov eax,[eax+edi*4+0B4h]
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
// clang-format on

ID3DBlob* pBlob = nullptr;
ID3DBlob* pErrorBlob = nullptr;
static int recreate_shader(float* shader_contrast) {
    EEShaderResourceSomething* ours = g_our_edited_shader_ptr;
    ID3D11PixelShader* new_ps{nullptr};

    auto device = g_framework->d3d11()->get_device();

    char buffer[2048] = {0};
    sprintf(buffer, shader_fmt, shader_contrast[0], shader_contrast[1], shader_contrast[2]);

    HRESULT hr = D3DCompile(buffer, ::strlen(buffer), nullptr, nullptr, nullptr, "main", "ps_5_0", 0, 0, &pBlob, &pErrorBlob);

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
    g_shader_contrast[0] = cfg.get<float>("shaderContrastR").value_or(1.09f);
    g_shader_contrast[1] = cfg.get<float>("shaderContrastG").value_or(1.09f);
    g_shader_contrast[2] = cfg.get<float>("shaderContrastB").value_or(1.09f);
    recreate_shader(g_shader_contrast);
}

void ShaderEdit::on_config_save(utility::Config& cfg) {
    cfg.set<float>("shaderContrastR", g_shader_contrast[0]);
    cfg.set<float>("shaderContrastG", g_shader_contrast[1]);
    cfg.set<float>("shaderContrastB", g_shader_contrast[2]);
}


// during load
//void ShaderEdit::on_config_load(const utility::Config &cfg) {}
// during save
//void ShaderEdit::on_config_save(utility::Config &cfg) {}
// do something every frame
//void ShaderEdit::on_frame() {}
// will show up in debug window, dump ImGui widgets you want here
//void ShaderEdit::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here

void ShaderEdit::on_draw_ui() {
    int shader_created = 0;

    // Calculate average contrast from g_shader_contrast
    float average_contrast           = (g_shader_contrast[0] + g_shader_contrast[1] + g_shader_contrast[2]) / 3.0f;
    static bool use_uniform_contrast = true;

    ImGui::Checkbox("Use uniform contrast", &use_uniform_contrast);

    if (use_uniform_contrast) {
        // Slider controls all three channels equally
        if (ImGui::SliderFloat("Adjust contrast", &average_contrast, 0.0f, 3.0f, "%.2f")) {
            g_shader_contrast[0] = average_contrast;
            g_shader_contrast[1] = average_contrast;
            g_shader_contrast[2] = average_contrast;

            shader_created = recreate_shader(g_shader_contrast);
        }
    } else {
        // Separate sliders for RGB contrast values
        if (ImGui::DragFloat3("Adjust contrast (RGB)", g_shader_contrast, 0.1f, 0.0f, 3.0f)) {
            shader_created = recreate_shader(g_shader_contrast);
        }
    }


    if (shader_created == 1) {
        ImGui::TextColored(ImColor::ImColor(0.3f, 1.0f, 0.3f, 1.0f), "Shader created!");
    }
    if (shader_created == -1) {
        ImGui::TextColored(ImColor::ImColor(1.0f, 0.3f, 0.3f, 1.0f), "Failed to create a shader!");
    }
}
