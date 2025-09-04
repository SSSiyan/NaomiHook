#include "ShaderEdit.hpp"
#include "SwordColours.hpp"
#include <atomic>
#include <d3dcompiler.h>

const ModToggle::Ptr g_mod_enabled{ModToggle::create("shader_edit_swap_shader_toggle", false)};

struct CompileTimeShaderDefines {
    bool disable_cc;
    bool contrast_fix;
    bool radial_blur_fix;
    int radial_blur_samples;
    bool radial_blur_speed_lines;
    float radial_blur_speed_lines_factor;
};

static CompileTimeShaderDefines g_comptime_shader_defs{
#ifndef NDEBUG
    .disable_cc                     = false,
    .contrast_fix                   = true,
    .radial_blur_fix                = true,
    .radial_blur_samples            = 12,
    .radial_blur_speed_lines        = true,
    .radial_blur_speed_lines_factor = 0.17f,
#else
    0
#endif
};

static int recreate_shader(ID3D11Device* device, CompileTimeShaderDefines& defs);
static ID3D11Device* g_d3d11_device{NULL};

inline const uint32_t hash_32_fnv1a(const void* key, const uint32_t len) {
    const char* data = (const char*)key;
    uint32_t hash    = 0x811c9dc5;
    uint32_t prime   = 0x1000193;
    for (uint32_t i = 0; i < len; ++i) {
        uint8_t value = (uint8_t)data[i];
        hash          = hash ^ value;
        hash *= prime;
    }
    return hash;
}

/* ===========================================================
   Replacement HLSL (ASCII safe)
   Adds:
     - kFx (0..1) master intensity
     - runtime CA toggle via kChromaOn (0/1)
     - derives strength/mix/speedlines from base values
   =========================================================== */
static const char* replace_test = R"(// ---- 3Dmigoto template (with kFx + CA toggle)

cbuffer PixelShaderConsts : register(b0)
{
    float4 regColor[4] : packoffset(c0);
    float4 konstColor[4] : packoffset(c4);
    float4 misc : packoffset(c8);
    float4 indirectOffsetScale1[3] : packoffset(c9);
    float4 indirectOffsetScale2[3] : packoffset(c12);
    float4 fogLimits : packoffset(c15);
    float4 fogColor : packoffset(c16);
}

cbuffer ContrastCB : register(b1)
{
    float4 kContrast;
    float4 kSMHDarkening;
}

/* b2 layout (16-byte packing):
   12 floats (48 bytes) + float3 (12) = 64 bytes total
*/
cbuffer RadialUserCB : register(b2)
{
    float kStrengthBase;      // c0.x
    float kChroma;            // c0.y  (amount when CA is on)
    float kInner;             // c0.z
    float kOuter;             // c0.w

    float kFalloff;           // c1.x
    float kMixBase;           // c1.y
    float kTwistDeg;          // c1.z
    float kTime;              // c1.w

    float kSpeedLinesBase;    // c2.x
    float kFx;                // c2.y (0..1)
    float kChromaOn;          // c2.z (0/1)
    float _pad0;              // c2.w

    float3 kSpeedLinesColor;  // c3.xyz
}

SamplerState texSampler0_s : register(s0);
SamplerState texSampler1_s : register(s1);
SamplerState texSampler2_s : register(s2);
Texture2D<float4> tex0 : register(t0);
Texture2D<float4> tex1 : register(t1);
Texture2D<float4> tex2 : register(t2);

static const float kGammaOn = 1.0;
static const float kGamma = 1.0;
static const float2 kCenterUV = float2(0.5,0.5);

#define DISABLE_CC %d
#define CONTRAST_FIX %d
#define RADIAL_BLUR_FIX %d
#define RADIAL_BLUR_SAMPLES %d
#define ANIME_SPEED_LINES %d
#define ANIME_SPEED_BLOOM %f

#define cmp -

#if ANIME_SPEED_LINES
float hash21(float2 p)
{
    float3 p3 = frac(float3(p.x, p.y, p.x) * 0.1031);
    p3 += dot(p3, p3.yzx + 33.33);
    return frac((p3.x + p3.y) * p3.z);
}

float snoise2D(float2 x)
{
    float2 i = floor(x);
    float2 f = frac(x);
    float a = hash21(i);
    float b = hash21(i + float2(1.0, 0.0));
    float c = hash21(i + float2(0.0, 1.0));
    float d = hash21(i + float2(1.0, 1.0));
    float2 u = f * f * (3.0 - 2.0 * f);
    return lerp(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}

float mirror(float t, float r)
{
    t = frac(t + r);
    return 2.0 * abs(t - 0.5);
}

float bloom_coeff(float dist)
{
    return max(ANIME_SPEED_BLOOM / (dist + 0.05) - 0.05, 0.0);
}

float radial_noise(float t, float d)
{
    const float SCALE = 95.0;
    d = pow(d, 0.004);
    float doff = kTime * 0.07;
    float2 p = float2(mirror(t, 0.1), d + doff);
    float f1 = bloom_coeff(snoise2D(p * SCALE));
    p = 2.1 * float2(mirror(t, 0.4), d + doff);
    float f2 = bloom_coeff(snoise2D(p * SCALE));
    p = 3.7 * float2(mirror(t, 0.8), d + doff);
    float f3 = bloom_coeff(snoise2D(p * SCALE));
    p = 5.8 * float2(mirror(t, 0.0), d + doff);
    float f4 = bloom_coeff(snoise2D(p * SCALE));
    return pow((f1 + 0.35 * f2 + 0.25 * f3 + 0.125 * f4) * 3.0, 1.0);
}

float3 colorize(float x)
{
    x = clamp(x, 0.0, 1.0);
    float3 c = kSpeedLinesColor.rgb;
    c = lerp(c, float3(1.0, 1.0, 1.0), x * 4.0 - 3.0) * x;
    c = max(c, float3(0.0, 0.0, 0.0));
    c = lerp(c, float3(1.0, 0.25, 1.0), smoothstep(1.0, 0.2, x) * smoothstep(0.15, 0.9, x));
    return c;
}

float3 speed_lines(float2 uv, float slScale)
{
    float d = dot(uv, uv);
    float t = atan2(uv.y, uv.x) / 6.28318530718;
    float v = radial_noise(t, d);
    v = v * 2.5 - 1.4;
    v = lerp(0.0, v, 0.8 * smoothstep(0.0, 2.7, d) * slScale);
    return colorize(v);
}
#endif

#if RADIAL_BLUR_FIX
float2 rot2(float2 v, float a)
{
    float s = sin(a), c = cos(a);
    return float2(c * v.x - s * v.y, s * v.x + c * v.y);
}

float4 radial_blur(float2 uvs, float strengthPx, float mixVal)
{
    float2 uv = uvs;
    float4 base = tex0.Sample(texSampler0_s, uv);

    float2 dudx = ddx(uv);
    float2 dudy = ddy(uv);
    float texel = max((abs(dudx.x) + abs(dudy.y)) * 0.5, 1e-6);

    float2 toC = uv - kCenterUV;
    float r = length(toC);
    float r01 = 0.0;
    if (kOuter > kInner + 1e-6)
    {
        r01 = saturate((r - kInner) / (kOuter - kInner));
    }
    else
    {
        r01 = 1.0;
    }

    float2 dir = (r > 1e-6) ? (toC / r) : float2(0.0, 0.0);
    float angTotal = radians(kTwistDeg) * r01;
    float stepUV = strengthPx * texel * r01;

    float3 acc = 0.0.xxx;
    float wsum = 0.0;

    float baseW = pow(1.0 - 0.0, kFalloff);
    acc += base.rgb * baseW;
    wsum += baseW;

    [unroll]
    for (int i = 1; i <= RADIAL_BLUR_SAMPLES; ++i)
    {
        float t = (float)i / (float)RADIAL_BLUR_SAMPLES;
        float w = pow(1.0 - t, kFalloff);
        float a = angTotal * t;
        float2 stepDir = rot2(dir, a);
        float2 ofs = stepDir * stepUV * t;

        if (kChromaOn > 0.5)
        {
            float2 uvR = uv + ofs * (1.0 + kChroma);
            float2 uvG = uv + ofs;
            float2 uvB = uv + ofs * (1.0 - kChroma);

            float3 s;
            s.r = tex0.Sample(texSampler0_s, uvR).r;
            s.g = tex0.Sample(texSampler0_s, uvG).g;
            s.b = tex0.Sample(texSampler0_s, uvB).b;

            acc += s * w;
        }
        else
        {
            float3 s = tex0.Sample(texSampler0_s, uv + ofs).rgb;
            acc += s * w;
        }

        wsum += w;
    }

    float3 blur = acc / max(wsum, 1e-6);
    float3 outRGB = lerp(base.rgb, blur, saturate(mixVal) * r01);

    if (kGammaOn > 0.5)
    {
        float g = max(kGamma, 1e-3);
        outRGB = pow(saturate(outRGB), 1.0 / g);
    }

    return float4(outRGB, base.a);
}
#endif

#if CONTRAST_FIX
float luminance(float3 color) {
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
float3 contrast_fix_cc(float3 col)
{
    float3 contrast = kContrast.xyz;
    col.rgb = (col.rgb - 0.5) * contrast + 0.5;
    float gray = luminance(col.rgb);
    col.rgb = lerp(float3(gray, gray, gray), col.rgb, kContrast.w);
    col.rgb = adjustLevels(col.rgb, kSMHDarkening.r, kSMHDarkening.g, kSMHDarkening.b);
    return col;
}
#endif

void main(
  float4 v0 : SV_POSITION0,
  float4 v1 : COLOR0,
  float4 v2 : COLOR1,
  float4 v3 : TEXCOORD0,
  float4 v4 : TEXCOORD1,
  float4 v5 : TEXCOORD2,
  out float4 o0 : SV_Target0,
  out float4 o1 : SV_Target1)
{
    float4 r0, r1;
    uint4 bitmask, uiDest;
    float4 fDest;

    // Derive per-frame values from base + kFx
    float fx01 = saturate(kFx);
    float strengthPx = kStrengthBase * pow(fx01, 0.8);
    float mixVal     = kMixBase * smoothstep(0.0, 1.0, fx01);
    float slScale    = kSpeedLinesBase * smoothstep(0.15, 0.85, fx01);

#if DISABLE_CC
    r0.w = cmp(v3.z != 0.000000);
    r1.xy = v3.xy / v3.zz;
    r1.xy = r0.ww ? r1.xy : v3.xy;
    float2 uv = r1.xy;

#if RADIAL_BLUR_FIX
    r1.xyz = radial_blur(uv, strengthPx, mixVal).xyz;
#else
    r1.xyz = tex0.Sample(texSampler0_s, uv).xyz;
#endif

#if CONTRAST_FIX
    r1.xyz = contrast_fix_cc(r1.xyz);
#endif

#if ANIME_SPEED_LINES
    float3 sl = speed_lines(uv - 0.5, slScale);
    r1.xyz += sl;
#endif

    o0.xyz = r1.xyz;
    o0.w = misc.x;
    o1.w = saturate(konstColor[0].w);
    return;
#else
    r0.x = cmp(v4.z != 0.000000);
    r0.yz = v4.xy / v4.zz;
    r0.xy = r0.xx ? r0.yz : v4.xy;
    r0.xyz = tex1.Sample(texSampler1_s, r0.xy).xyz;
    r0.xyz = saturate(konstColor[0].xyz * r0.xyz);
    r0.xyz = konstColor[0].www * r0.xyz;
    r0.w = cmp(v3.z != 0.000000);
    r1.xy = v3.xy / v3.zz;
    r1.xy = r0.ww ? r1.xy : v3.xy;
    float2 uv = r1.xy;

#if RADIAL_BLUR_FIX
    r1.xyz = radial_blur(uv, strengthPx, mixVal).xyz;
#else
    r1.xyz = tex0.Sample(texSampler0_s, uv).xyz;
#endif

#if CONTRAST_FIX
    r1.xyz = contrast_fix_cc(r1.xyz);
#endif

    r0.w = 1 + -konstColor[0].w;
    r0.xyz = saturate(r1.xyz * r0.www + r0.xyz);
    r0.w = cmp(v5.z != 0.000000);
    r1.xy = v5.xy / v5.zz;
    r1.xy = r0.ww ? r1.xy : v5.xy;
    r1.xyz = tex2.Sample(texSampler2_s, r1.xy).xyz;
    r1.xyz = saturate(r1.xyz * r0.xyz);
    r1.xyz = konstColor[1].www * r1.xyz;
    r0.w = 1 + -konstColor[1].w;
    o0.xyz = saturate(r0.xyz * r0.www + r1.xyz);

#if ANIME_SPEED_LINES
    float3 sl = speed_lines(uv - 0.5, slScale);
    o0.xyz += sl;
#endif

    o0.w = misc.x;
    o1.xyz = float3(0.0, 0.0, 0.0);
    o1.w = saturate(konstColor[0].w);
    return;
#endif
}
)";

/* ===========================================================
   Engine hooks (same as before)
   =========================================================== */
static std::unique_ptr<FunctionHook> g_pipeline_create_shader_hook;

static ID3D11PixelShader* g_paper_cc_pixel_shader_game = nullptr;
static ID3D11PixelShader* g_paper_cc_pixel_shader_ours = nullptr;

HRESULT __stdcall d3d11_create_ps(ID3D11Device* device, const void* pshader_bytecode, SIZE_T bytecode_length,
    ID3D11ClassLinkage* p_class_linkage, ID3D11PixelShader** p_pixel_shader_out) {
    if (bytecode_length == 2384) {
        std::uint32_t hash = hash_32_fnv1a(pshader_bytecode, (uint32_t)bytecode_length);
        spdlog::info("[ShaderEdit] CreatePixelShader hash=%u", hash);
        if (hash == 0x52BFE5DF) {
            (void)recreate_shader(device, g_comptime_shader_defs);
            HRESULT ret = device->CreatePixelShader(pshader_bytecode, bytecode_length, p_class_linkage, p_pixel_shader_out);
            g_paper_cc_pixel_shader_game = (*p_pixel_shader_out);
            return ret;
        }
    }
    return device->CreatePixelShader(pshader_bytecode, bytecode_length, p_class_linkage, p_pixel_shader_out);
}

// clang-format off
static uintptr_t pipeline_cache_create_ps_jmp_back {NULL};
naked void pipeline_cache_create_ps_detour() {
    __asm {
        call d3d11_create_ps
        mov eax, [ebp - 18h]
        jmp dword ptr [pipeline_cache_create_ps_jmp_back]
    }
}
// clang-format on

/* ===========================================================
   Constant buffers and runtime data
   =========================================================== */
static ID3D11Buffer* g_radial_cb = nullptr;
static ID3D11Buffer* g_ccsfix_cb = nullptr;

class RadialCB {
public:
    // Base values the shader scales by kFx
    float kStrengthBase; // 0x00
    float kChroma;       // 0x04
    float kInner;        // 0x08
    float kOuter;        // 0x0C

    float kFalloff;  // 0x10
    float kMixBase;  // 0x14
    float kTwistDeg; // 0x18
    float kTime;     // 0x1C

    float kSpeedLinesBase; // 0x20
    float kFx;             // 0x24
    float kChromaOn;       // 0x28   // 0 or 1
    float _pad0;           // 0x2C   // align to 16

    glm::vec3 kSpeedLinesColor; // 0x30
};                              // total 0x3C, but D3D cbuffer rounds up to 0x40
static_assert(sizeof(RadialCB) == 0x3C || sizeof(RadialCB) == 0x40, "RadialCB layout");
static RadialCB g_radial_cb_data{};

struct ContrastCB {
    glm::vec4 kContrast;
    glm::vec4 kSMHDarkening;
};
static_assert(sizeof(ContrastCB) == 0x20, "ContrastCB must be 0x20 bytes");
static ContrastCB g_ccsfix_cb_data{};

static bool g_preview_flag{false};

/* Easing state */
static float g_fx_value   = 0.0f;   // current eased intensity (0..1)
static double g_fx_last_t = 0.0;    // last time sample
static float g_tau_in_ms  = 100.0f; // ease-in time constant (ms)
static float g_tau_out_ms = 180.0f; // ease-out time constant (ms)

/* ===========================================================
   Update/bind constant buffers
   =========================================================== */
static void update_constant_buffers() {
    if (!g_radial_cb || !g_ccsfix_cb)
        return;
    auto device = g_d3d11_device;
    if (!device)
        return;

    ID3D11DeviceContext* ctx = nullptr;
    device->GetImmediateContext(&ctx);
    if (!ctx)
        return;

    // Contrast CB
    ctx->UpdateSubresource(g_ccsfix_cb, 0, nullptr, &g_ccsfix_cb_data, 0, 0);

    // Radial CB + EASING
    {
        double now  = ImGui::GetTime();
        double dt   = (g_fx_last_t > 0.0) ? (now - g_fx_last_t) : 0.0;
        g_fx_last_t = now;

        // Gameplay target: if deathblow timer > 0 => full on, else off
        float timer  = (float)SwordColours::deathblowTimer;
        float target = (timer > 0.0f) ? 1.0f : 0.0f;

        // Allow preview to override target with slider-like behavior
        if (g_preview_flag) {
            // When previewing, we keep target as-is (use kFx already set via UI if needed)
            // Here we just keep easing toward whatever target is (still 0/1 by default).
        }

        // Time constants
        float tau = (target > g_fx_value) ? (g_tau_in_ms * 0.001f) : (g_tau_out_ms * 0.001f);
        float a   = 0.0f;
        if (tau > 0.0001f && dt > 0.0) {
            a = 1.0f - (float)exp(-(float)dt / tau);
        } else {
            a = 1.0f; // immediate if no time has passed or tau ~ 0
        }
        g_fx_value += (target - g_fx_value) * a;
        if (g_fx_value < 0.0f)
            g_fx_value = 0.0f;
        if (g_fx_value > 1.0f)
            g_fx_value = 1.0f;

        // Stamp time for the shader noise
        g_radial_cb_data.kTime = (float)now;

        // Write current eased intensity
        g_radial_cb_data.kFx = g_fx_value;

        ctx->UpdateSubresource(g_radial_cb, 0, nullptr, &g_radial_cb_data, 0, 0);
    }

    // Let engine bind; or bind explicitly if you prefer:
    ctx->PSSetConstantBuffers(1, 1, &g_ccsfix_cb);
    ctx->PSSetConstantBuffers(2, 1, &g_radial_cb);
}

static void ensure_constant_buffers_exist() {
    if (g_radial_cb && g_ccsfix_cb) {
        update_constant_buffers();
        return;
    }
    auto device = g_d3d11_device;
    if (!device)
        return;

    int ok = 0;

    { // Contrast
        D3D11_BUFFER_DESC desc{};
        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        desc.Usage     = D3D11_USAGE_DEFAULT;
        desc.ByteWidth = (UINT)sizeof(ContrastCB);
        if (SUCCEEDED(device->CreateBuffer(&desc, nullptr, &g_ccsfix_cb)))
            ok += 1;
    }
    { // Radial
        D3D11_BUFFER_DESC desc{};
        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        desc.Usage     = D3D11_USAGE_DEFAULT;
        // Round up to 16-byte multiple for safety
        desc.ByteWidth = ((UINT)sizeof(RadialCB) + 15u) & ~15u;
        if (SUCCEEDED(device->CreateBuffer(&desc, nullptr, &g_radial_cb)))
            ok += 1;
    }

    if (ok == 2)
        update_constant_buffers();
}

static void update_shader_constants() {
    ensure_constant_buffers_exist();
}

/* ===========================================================
   PS bind hook
   =========================================================== */
static uintptr_t set_ps_shader_jmp_back{NULL};
static void __stdcall ps_set_shader_ours(ID3D11DeviceContext* ctx, ID3D11PixelShader* ps, ID3D11ClassInstance* const* cl, UINT cls) {
    if (g_mod_enabled->value() && (ps == g_paper_cc_pixel_shader_game)) {
        ctx->PSSetShader(g_paper_cc_pixel_shader_ours, cl, cls);
        update_shader_constants();
        return;
    }
    ctx->PSSetShader(ps, cl, cls);
}

// clang-format off
naked void set_ps_shader_detour() {
    __asm {
        call ps_set_shader_ours
    originalCode:
        mov eax, 8h
        jmp dword ptr [set_ps_shader_jmp_back]
    }
}
// clang-format on

/* ===========================================================
   Shader (re)creation
   =========================================================== */
static int recreate_shader(ID3D11Device* device, CompileTimeShaderDefines& defs) {
    static char local_buf[65536];
    int samples = defs.radial_blur_samples;
    if (samples < 4)
        samples = 4;
    if (samples > 64)
        samples = 64;

    if (!g_d3d11_device)
        g_d3d11_device = device;

    int n = snprintf(local_buf, sizeof(local_buf), replace_test, (int)defs.disable_cc, (int)defs.contrast_fix, (int)defs.radial_blur_fix,
        samples, (int)defs.radial_blur_speed_lines, defs.radial_blur_speed_lines_factor);
    if (n <= 0) {
        spdlog::error("[ShaderEdit] snprintf failed");
        return -1;
    }

    ID3DBlob* pBlob = nullptr;
    ID3DBlob* pErr  = nullptr;

    HRESULT hr = D3DCompile(local_buf, (SIZE_T)strlen(local_buf), nullptr, nullptr, nullptr, "main", "ps_5_0", 0, 0, &pBlob, &pErr);
    if (FAILED(hr)) {
        if (pErr) {
            OutputDebugStringA((char*)pErr->GetBufferPointer());
            pErr->Release();
        }
        return -1;
    }

    if (g_paper_cc_pixel_shader_ours) {
        g_paper_cc_pixel_shader_ours->Release();
        g_paper_cc_pixel_shader_ours = nullptr;
    }

    HRESULT ret = device->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &g_paper_cc_pixel_shader_ours);
    if (FAILED(ret)) {
        spdlog::error("[ShaderEdit] CreatePixelShader failed for replacement");
        return -1;
    }
    return 1;
}

/* ===========================================================
   Mod lifecycle
   =========================================================== */
std::optional<std::string> ShaderEdit::on_initialize() {
    static bool initialized = false;
    if (initialized)
        return Mod::on_initialize();

    if (!install_hook_offset(
            0x36A3B, g_pipeline_create_shader_hook, pipeline_cache_create_ps_detour, &pipeline_cache_create_ps_jmp_back, 6)) {
        return "Failed to install create ps hook";
    }

    if (!install_hook_offset(0x372D8, m_set_ps_hook, set_ps_shader_detour, &set_ps_shader_jmp_back, 8)) {
        return "Failed to install after rt hook detour";
    }

    initialized = true;
    return Mod::on_initialize();
}

/* ===========================================================
   Config load/save (defaults and UI)
   =========================================================== */
void ShaderEdit::on_config_load(const utility::Config& cfg) {
    // Contrast defaults
    g_ccsfix_cb_data.kContrast =
        glm::vec4(cfg.get<float>("shaderContrastR").value_or(1.09f), cfg.get<float>("shaderContrastG").value_or(1.09f),
            cfg.get<float>("shaderContrastB").value_or(1.09f), cfg.get<float>("shaderSaturation").value_or(1.0f));
    g_ccsfix_cb_data.kSMHDarkening = glm::vec4(cfg.get<float>("shadowDarkening").value_or(1.0f),
        cfg.get<float>("midtoneDarkening").value_or(1.0f), cfg.get<float>("highlightDarkening").value_or(1.0f), 0.0f);

    // Compile-time toggles
    g_comptime_shader_defs.contrast_fix                   = cfg.get<bool>("shaderContrastFixEnabled").value_or(true);
    g_comptime_shader_defs.disable_cc                     = cfg.get<bool>("shaderColorCorectionDisabled").value_or(false);
    g_comptime_shader_defs.radial_blur_fix                = cfg.get<bool>("shaderRadialBlurFix").value_or(true);
    g_comptime_shader_defs.radial_blur_samples            = cfg.get<int>("shaderRadialBlurSamples").value_or(12);
    g_comptime_shader_defs.radial_blur_speed_lines        = cfg.get<bool>("shaderRadialBlurAnimeSpeedLines").value_or(true);
    g_comptime_shader_defs.radial_blur_speed_lines_factor = cfg.get<float>("shaderRadialBlurSpeedLinesFactor").value_or(0.18f);

    // Radial base defaults (tunable)
    g_radial_cb_data.kStrengthBase    = cfg.get<float>("shaderRadialStrengthBase").value_or(160.0f);
    g_radial_cb_data.kMixBase         = cfg.get<float>("shaderRadialMixBase").value_or(1.00f);
    g_radial_cb_data.kInner           = cfg.get<float>("shaderRadialInner").value_or(0.00f);
    g_radial_cb_data.kOuter           = cfg.get<float>("shaderRadialOuter").value_or(0.24f);
    g_radial_cb_data.kFalloff         = cfg.get<float>("shaderRadialFalloff").value_or(3.54f);
    g_radial_cb_data.kTwistDeg        = cfg.get<float>("shaderRadialTwistDeg").value_or(0.0f);
    g_radial_cb_data.kChroma          = cfg.get<float>("shaderRadialChromaAmount").value_or(0.00f);
    g_radial_cb_data.kSpeedLinesBase  = cfg.get<float>("shaderRadialSpeedLinesBase").value_or(1.0f);
    g_radial_cb_data.kChromaOn        = cfg.get<bool>("shaderRadialChromaOn").value_or(false) ? 1.0f : 0.0f;
    g_radial_cb_data.kSpeedLinesColor = glm::vec3(1.0f, 1.0f, 1.0f);
    g_radial_cb_data.kTime            = 0.0f;
    g_radial_cb_data.kFx              = 0.0f;
    g_radial_cb_data._pad0            = 0.0f;

    // Easing constants
    g_tau_in_ms  = cfg.get<float>("shaderFxTauInMs").value_or(100.0f);
    g_tau_out_ms = cfg.get<float>("shaderFxTauOutMs").value_or(180.0f);

    g_mod_enabled->config_load(cfg);
}

void ShaderEdit::on_config_save(utility::Config& cfg) {
    // Contrast
    cfg.set<float>("shaderContrastR", g_ccsfix_cb_data.kContrast.r);
    cfg.set<float>("shaderContrastG", g_ccsfix_cb_data.kContrast.g);
    cfg.set<float>("shaderContrastB", g_ccsfix_cb_data.kContrast.b);
    cfg.set<float>("shaderSaturation", g_ccsfix_cb_data.kContrast.w);
    cfg.set<float>("shadowDarkening", g_ccsfix_cb_data.kSMHDarkening.r);
    cfg.set<float>("midtoneDarkening", g_ccsfix_cb_data.kSMHDarkening.g);
    cfg.set<float>("highlightDarkening", g_ccsfix_cb_data.kSMHDarkening.b);

    // Compile-time toggles
    cfg.set<bool>("shaderContrastFixEnabled", g_comptime_shader_defs.contrast_fix);
    cfg.set<bool>("shaderColorCorectionDisabled", g_comptime_shader_defs.disable_cc);
    cfg.set<bool>("shaderRadialBlurFix", g_comptime_shader_defs.radial_blur_fix);
    cfg.set<int>("shaderRadialBlurSamples", g_comptime_shader_defs.radial_blur_samples);
    cfg.set<bool>("shaderRadialBlurAnimeSpeedLines", g_comptime_shader_defs.radial_blur_speed_lines);
    cfg.set<float>("shaderRadialBlurSpeedLinesFactor", g_comptime_shader_defs.radial_blur_speed_lines_factor);

    // Radial base
    cfg.set<float>("shaderRadialStrengthBase", g_radial_cb_data.kStrengthBase);
    cfg.set<float>("shaderRadialMixBase", g_radial_cb_data.kMixBase);
    cfg.set<float>("shaderRadialInner", g_radial_cb_data.kInner);
    cfg.set<float>("shaderRadialOuter", g_radial_cb_data.kOuter);
    cfg.set<float>("shaderRadialFalloff", g_radial_cb_data.kFalloff);
    cfg.set<float>("shaderRadialTwistDeg", g_radial_cb_data.kTwistDeg);
    cfg.set<float>("shaderRadialChromaAmount", g_radial_cb_data.kChroma);
    cfg.set<float>("shaderRadialSpeedLinesBase", g_radial_cb_data.kSpeedLinesBase);
    cfg.set<bool>("shaderRadialChromaOn", g_radial_cb_data.kChromaOn > 0.5f);
    cfg.set<float>("shaderFxTauInMs", g_tau_in_ms);
    cfg.set<float>("shaderFxTauOutMs", g_tau_out_ms);

    g_mod_enabled->config_save(cfg);
}

/* ===========================================================
   UI
   =========================================================== */
static inline void PushRuntimeCBs() {
    update_shader_constants();
}

static void ImGoo_CCheckbox(const char* label, bool* v) {
    if (ImGui::Checkbox(label, v)) {
        recreate_shader(g_d3d11_device, g_comptime_shader_defs);
        PushRuntimeCBs();
    }
}
static void ImGoo_CInt(const char* label, int* v) {
    if (ImGui::SliderInt(label, v, 4, 64)) {
        recreate_shader(g_d3d11_device, g_comptime_shader_defs);
        PushRuntimeCBs();
    }
}
static void ImGoo_CFloat(const char* label, float* v, float lo = 0.0f, float hi = 1.8f, const char* fmt = "%.3f") {
    if (ImGui::SliderFloat(label, v, lo, hi, fmt)) {
        PushRuntimeCBs();
    }
}

void ShaderEdit::on_draw_ui() {
    g_mod_enabled->draw("Enable the thing?");
    if (!g_mod_enabled->value())
        return;

    if (ImGui::TreeNode("Compile time shader settings")) {
        ImGoo_CCheckbox("Diable Color Correction?", &g_comptime_shader_defs.disable_cc);
        ImGoo_CCheckbox("Enable contrast fix?", &g_comptime_shader_defs.contrast_fix);
        ImGoo_CCheckbox("Enable radial blur fix?", &g_comptime_shader_defs.radial_blur_fix);
        if (g_comptime_shader_defs.radial_blur_fix) {
            ImGoo_CInt("Number of samples [4-64]", &g_comptime_shader_defs.radial_blur_samples);
        }
        ImGoo_CCheckbox("Enable speed lines?", &g_comptime_shader_defs.radial_blur_speed_lines);
        if (g_comptime_shader_defs.radial_blur_speed_lines) {
            ImGoo_CFloat("Speed lines factor", &g_comptime_shader_defs.radial_blur_speed_lines_factor, 0.0f, 1.8f, "%.3f");
        }
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Contrast settings")) {
        if (ImGui::SliderFloat3("Contrast (RGB)", (float*)&g_ccsfix_cb_data.kContrast, 0.0f, 3.0f))
            PushRuntimeCBs();
        if (ImGui::SliderFloat3("Shadows/Midtones/Highlights", (float*)&g_ccsfix_cb_data.kSMHDarkening, 0.0f, 3.0f))
            PushRuntimeCBs();
        if (ImGui::SliderFloat("Saturation", (float*)&g_ccsfix_cb_data.kContrast.w, 0.0f, 3.0f))
            PushRuntimeCBs();
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Radial Blur Settings")) {
        ImGui::Checkbox("Preview mode (hold state)", &g_preview_flag);

        // Base values (shader scales them by kFx)
        ImGoo_CFloat("Strength base (px)", &g_radial_cb_data.kStrengthBase, 0.0f, 300.0f, "%.1f");
        ImGoo_CFloat("Mix base (0..1)", &g_radial_cb_data.kMixBase, 0.0f, 1.0f, "%.2f");
        ImGoo_CFloat("Inner radius", &g_radial_cb_data.kInner, 0.0f, 1.0f, "%.2f");
        ImGoo_CFloat("Outer radius", &g_radial_cb_data.kOuter, 0.0f, 1.0f, "%.2f");
        ImGoo_CFloat("Falloff (shape)", &g_radial_cb_data.kFalloff, 0.1f, 8.0f, "%.2f");
        ImGoo_CFloat("Twist at edge (deg)", &g_radial_cb_data.kTwistDeg, -360.0f, 360.0f, "%.1f");

        // Chromatic aberration toggle + amount
        {
            bool on = (g_radial_cb_data.kChromaOn > 0.5f);
            if (ImGui::Checkbox("Chromatic aberration ON", &on)) {
                g_radial_cb_data.kChromaOn = on ? 1.0f : 0.0f;
                PushRuntimeCBs();
            }
            if (on) {
                ImGoo_CFloat("CA Amount", &g_radial_cb_data.kChroma, 0.0f, 0.5f, "%.3f");
            }
        }

        // Speed lines base scale + color
        ImGoo_CFloat("Speed lines base scale", &g_radial_cb_data.kSpeedLinesBase, 0.0f, 1.0f, "%.2f");
        if (ImGui::ColorEdit3("Speed lines color", (float*)&g_radial_cb_data.kSpeedLinesColor))
            PushRuntimeCBs();

        // Easing controls
        if (ImGui::TreeNode("Easing")) {
            ImGoo_CFloat("Tau in (ms)", &g_tau_in_ms, 10.0f, 500.0f, "%.0f");
            ImGoo_CFloat("Tau out (ms)", &g_tau_out_ms, 10.0f, 500.0f, "%.0f");
            ImGui::Text("kFx eased: %.3f", g_fx_value);
            ImGui::TreePop();
        }

        ImGui::TreePop();
    }

    // live push
    PushRuntimeCBs();
}
