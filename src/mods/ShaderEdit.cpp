#include "ShaderEdit.hpp"
#include "SwordColours.hpp"
#include <d3dcompiler.h>

#define BLENDOR_SYNC 0

const ModToggle::Ptr g_mod_enabled{ModToggle::create("shader_edit_swap_shader_toggle", false)};

struct CompileTimeShaderDefines {
    bool disable_cc;
    bool contrast_fix;
    bool radial_blur_fix;
    int radial_blur_samples;
    bool radial_blur_speed_lines;
    float radial_blur_speed_lines_factor;
    float radial_blur_strength_pixels;
};

static CompileTimeShaderDefines g_comptime_shader_defs{
    .disable_cc                     = false,
    .contrast_fix                   = true,
    .radial_blur_fix                = true,
    .radial_blur_samples            = 12,
    .radial_blur_speed_lines        = true,
    .radial_blur_speed_lines_factor = 0.17f,
    .radial_blur_strength_pixels    = 160.0f,
};

static int recreate_shader(ID3D11Device* device, CompileTimeShaderDefines& defs);
static ID3D11Device* g_d3d11_device{NULL};

inline const uint32_t hash_32_fnv1a(const void* key, const uint32_t len) {

    const char* data = (char*)key;
    uint32_t hash    = 0x811c9dc5;
    uint32_t prime   = 0x1000193;

    for (uint32_t i = 0; i < len; ++i) {
        uint8_t value = data[i];
        hash          = hash ^ value;
        hash *= prime;
    }

    return hash;

} // hash_32_fnv1a

static const char* replace_test = R"(
// ---- Created with 3Dmigoto v1.3.16 on Tue Aug 19 17:26:44 2025

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

cbuffer RadialUserCB : register(b2)
{
    float kStrengthPx; // pixels at outer radius
    float kChroma;
    float kInner;
    float kOuter;
    float kFalloff;
    float kMix;
    float kTwistDeg;
    float kTime;
    float kSpeedLinesScale;
    float3 kSpeedLinesColor;
}

SamplerState texSampler0_s : register(s0);
SamplerState texSampler1_s : register(s1);
SamplerState texSampler2_s : register(s2);
Texture2D<float4> tex0 : register(t0);
Texture2D<float4> tex1 : register(t1);
Texture2D<float4> tex2 : register(t2);

// Gamma controls (shared toggle)
static const float kGammaOn = 1.0; // 0 or 1
static const float kGamma = 1.0; // 0.1..3.0
static const float2 kCenterUV = float2(0.5,0.5);

// Compile-time sample count

#define DISABLE_CC %d

#define CONTRAST_FIX %d

#define RADIAL_BLUR_FIX %d
#define RADIAL_BLUR_SAMPLES %d

#define ANIME_SPEED_LINES %d
#define ANIME_SPEED_BLOOM %f

// 3Dmigoto declarations
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
    float3 c = float3(0.78, 0.78, 0.78); //kSpeedLinesColor.rgb;
    c = lerp(c, float3(1.0, 1.0, 1.0), x * 4.0 - 3.0) * x;
    c = max(c, float3(0.0, 0.0, 0.0));
    c = lerp(c, float3(1.0, 0.25, 1.0), smoothstep(1.0, 0.2, x) * smoothstep(0.15, 0.9, x));
    return c * kSpeedLinesColor.rgb;
}

float3 speed_lines(float2 uv)
{
    float d = dot(uv, uv);
    float t = atan2(uv.y, uv.x) / 6.28318530718;
    float v = radial_noise(t, d);
    v = v * 2.5 - 1.4;
    v = lerp(0.0, v, 0.8 * smoothstep(0.0, 2.7, d) * kSpeedLinesScale);
    return colorize(v);
}
#endif

#if RADIAL_BLUR_FIX

float2 rot2(float2 v, float a)
{
    float s = sin(a), c = cos(a);
    return float2(c * v.x - s * v.y, s * v.x + c * v.y);
}
#if !(DISABLE_CC)
// can't pass texture here i think so have to do this cringe
float4 radial_blur_bw(float2 uvs)
{
    float2 uv = uvs;

    // Base color
    float4 base = tex1.Sample(texSampler0_s, uv);

    // Estimate texel size from derivatives (fullscreen triangle should give 1/width,1/height)
    float2 dudx = ddx(uv);
    float2 dudy = ddy(uv);
    float texel = max((abs(dudx.x) + abs(dudy.y)) * 0.5, 1e-6);

    // Compute radial info
    float2 toC = uv - kCenterUV;
    float r = length(toC);
    float r01 = 0.0;
    if (kOuter > kInner + 1e-6)
    {
        r01 = saturate((r - kInner) / (kOuter - kInner));
    }
    else
    {
        r01 = 1.0; // if outer == inner, apply everywhere
    }

    float2 dir = (r > 1e-6) ? (toC / r) : float2(0.0, 0.0);
    //float2 st = float2(atan2(dir.x, dir.y), length(dir.y));
    //float c = frac( sin( * 100.0) * 5670.0);
    //return float4(c.xxxx);

    // Twist grows toward edge
    float angTotal = radians(kTwistDeg) * r01;

    // Convert pixel strength to UV and scale by r01 so it is zero at center band
    float stepUV = kStrengthPx * texel * r01;

    // Accumulate taps
    float3 acc = 0.0.xxx;
    float wsum = 0.0;

    // Include the base pixel to keep energy stable
    float baseW = pow(1.0 - 0.0, kFalloff);
    acc += base.rgb * baseW;
    wsum += baseW;

    [unroll]
    for (int i = 1; i <= RADIAL_BLUR_SAMPLES; ++i)
    {
        float t = (float) i / (float) RADIAL_BLUR_SAMPLES; // 0..1
        float w = pow(1.0 - t, kFalloff); // near center gets more weight
        float a = angTotal * t; // progressive twist

        float2 stepDir = rot2(dir, a);
        float2 ofs = stepDir * stepUV * t;

        float2 uvR = uv + ofs;
        float2 uvG = uv + ofs;
        float2 uvB = uv + ofs;

        float3 s;
        s.r = tex1.Sample(texSampler0_s, uvR).r;
        s.g = tex1.Sample(texSampler0_s, uvG).g;
        s.b = tex1.Sample(texSampler0_s, uvB).b;

        acc += s * w;
        wsum += w;
    }

    float3 blur = acc / max(wsum, 1e-6);

    // Screen-space mix with original, scaled by r01 (fade toward center band)
    float3 outRGB = lerp(base.rgb, blur, saturate(kMix) * r01);

    // Optional gamma after effect
    if (kGammaOn > 0.5)
    {
        float g = max(kGamma, 1e-3);
        outRGB = pow(saturate(outRGB), 1.0 / g);
    }

    return float4(outRGB, base.a);
}
#endif

float4 radial_blur_rgb(float2 uvs)
{
    float2 uv = uvs;

    // Base color
    float4 base = tex0.Sample(texSampler0_s, uv);

    // Estimate texel size from derivatives (fullscreen triangle should give 1/width,1/height)
    float2 dudx = ddx(uv);
    float2 dudy = ddy(uv);
    float texel = max((abs(dudx.x) + abs(dudy.y)) * 0.5, 1e-6);

    // Compute radial info
    float2 toC = uv - kCenterUV;
    float r = length(toC);
    float r01 = 0.0;
    if (kOuter > kInner + 1e-6)
    {
        r01 = saturate((r - kInner) / (kOuter - kInner));
    }
    else
    {
        r01 = 1.0; // if outer == inner, apply everywhere
    }

    float2 dir = (r > 1e-6) ? (toC / r) : float2(0.0, 0.0);
    //float2 st = float2(atan2(dir.x, dir.y), length(dir.y));
    //float c = frac( sin( * 100.0) * 5670.0);
    //return float4(c.xxxx);

    // Twist grows toward edge
    float angTotal = radians(kTwistDeg) * r01;

    // Convert pixel strength to UV and scale by r01 so it is zero at center band
    float stepUV = kStrengthPx * texel * r01;

    // Accumulate taps
    float3 acc = 0.0.xxx;
    float wsum = 0.0;

    // Include the base pixel to keep energy stable
    float baseW = pow(1.0 - 0.0, kFalloff);
    acc += base.rgb * baseW;
    wsum += baseW;

    [unroll]
    for (int i = 1; i <= RADIAL_BLUR_SAMPLES; ++i)
    {
        float t = (float) i / (float) RADIAL_BLUR_SAMPLES; // 0..1
        float w = pow(1.0 - t, kFalloff); // near center gets more weight
        float a = angTotal * t; // progressive twist

        float2 stepDir = rot2(dir, a);
        float2 ofs = stepDir * stepUV * t;

        // Chromatic fringing: slight per-channel radius difference
        float2 uvR = uv + ofs * (1.0 + kChroma);
        float2 uvG = uv + ofs;
        float2 uvB = uv + ofs * (1.0 - kChroma);

        float3 s;
        s.r = tex0.Sample(texSampler0_s, uvR).r;
        s.g = tex0.Sample(texSampler0_s, uvG).g;
        s.b = tex0.Sample(texSampler0_s, uvB).b;

        acc += s * w;
        wsum += w;
    }

    float3 blur = acc / max(wsum, 1e-6);

    // Screen-space mix with original, scaled by r01 (fade toward center band)
    float3 outRGB = lerp(base.rgb, blur, saturate(kMix) * r01);

    // Optional gamma after effect
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

float3 contrast_fix_cc(float3 col)
{
    // Apply per-channel contrast
    float3 contrast = kContrast.xyz;
    col.rgb = (col.rgb - 0.5) * contrast + 0.5;

    // Apply saturation
    float gray = luminance(col.rgb);
    col.rgb = lerp(float3(gray, gray, gray), col.rgb, kContrast.w);

    // Apply shadows, midtones, highlights darkening
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

#if DISABLE_CC

    r0.w = cmp(v3.z != 0.000000);
    r1.xy = v3.xy / v3.zz;
    r1.xy = r0.ww ? r1.xy : v3.xy;
    float2 uv = r1.xy;

#if RADIAL_BLUR_FIX
    // radial blur
    r1.xyz = radial_blur_rgb(r1.xy).xyz;
#else
    r1.xyz = tex0.Sample(texSampler0_s, r1.xy).xyz;
#endif

#if CONTRAST_FIX
    // contrast fix
    r1.xyz = contrast_fix_cc(r1.xyz);
#endif

#if ANIME_SPEED_LINES
    // speed lines
    float3 sl = speed_lines(uv - 0.5);
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
#if RADIAL_BLUR_FIX
    r0.xyz = radial_blur_bw(r0.xy).xyz;
#else
    r0.xyz = tex1.Sample(texSampler1_s, r0.xy).xyz;
#endif
    r0.xyz = saturate(konstColor[0].xyz * r0.xyz);
    r0.xyz = konstColor[0].www * r0.xyz;
    r0.w = cmp(v3.z != 0.000000);
    r1.xy = v3.xy / v3.zz;
    r1.xy = r0.ww ? r1.xy : v3.xy;
    float2 uv = r1.xy;
#if RADIAL_BLUR_FIX
    r1.xyz = radial_blur_rgb(uv).xyz;
#else
    r1.xyz = tex0.Sample(texSampler0_s, r1.xy).xyz;
#endif

#if CONTRAST_FIX
    // contrast fix
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
    // speed lines
    float3 sl = speed_lines(uv - 0.5);
    o0.xyz += sl;
#endif
    // radial blur
    //r0.xyz = radial_blur_rgb(v3.xy).xyz;
    //o0.xyz = lerp(r0.xyz, o0.xyz, 0.0);
    o0.w = misc.x;
    o1.xyz = float3(0.0, 0.0, 0.0);
    o1.w = saturate(konstColor[0].w);
    return;
#endif
}
)";

static std::unique_ptr<FunctionHook> g_pipeline_create_shader_hook;

static ID3D11PixelShader* g_paper_cc_pixel_shader_game;
static ID3D11PixelShader* g_paper_cc_pixel_shader_ours;

HRESULT __stdcall d3d11_create_ps(ID3D11Device* device, const void* pshader_bytecode, SIZE_T bytecode_length,
    ID3D11ClassLinkage* p_class_linkage, ID3D11PixelShader** p_pixel_shader_out) {
    if (bytecode_length == 2384) {

        std::uint32_t hash = hash_32_fnv1a(pshader_bytecode, bytecode_length);
        spdlog::info("[ShaderEdit]: [CreatePixelShader] shader hashed to {}\n", hash);
        if (hash != 0x52BFE5DF) {
            goto early_exit;
        }

        int sret = recreate_shader(device, g_comptime_shader_defs);
        if (sret < 0) {
            spdlog::error("[ShaderEdit]: Failed to create our replacement shader!");
        }
        HRESULT ret                  = device->CreatePixelShader(pshader_bytecode, bytecode_length, p_class_linkage, p_pixel_shader_out);
        g_paper_cc_pixel_shader_game = (*p_pixel_shader_out);
        return ret;
    }

early_exit:
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

static ID3D11Buffer* g_radial_cb = nullptr;
static ID3D11Buffer* g_ccsfix_cb = nullptr;

class RadialCB {
public:
    float kStrengthPx;          // 0x0000
    float kChroma;              // 0x0004
    float kInner;               // 0x0008
    float kOuter;               // 0x000C
    float kFalloff;             // 0x0010
    float kMix;                 // 0x0014
    float kTwistDeg;            // 0x0018
    float kTime;                // 0x001C
    float kSpeedLinesScale;     // 0x0020
    glm::vec3 kSpeedLinesColor; // 0x0024
}; // Size: 0x0030
static_assert(sizeof(RadialCB) == 0x30);

struct ContrastCB {
    glm::vec4 kContrast;     // 0x0000
    glm::vec4 kSMHDarkening; // 0x000c
}; // Size: 0x0020
static_assert(sizeof(ContrastCB) == 0x20);

static RadialCB g_radial_cb_data{};
static ContrastCB g_ccsfix_cb_data{};

// to preview changes in imgoo window
static bool g_preview_flag{false};

#if BLENDOR_SYNC

#define SHM_NAME "Local\\BlenderToC_SharedMem"
#define SHM_SIZE sizeof(RadialCB)

#define EVENT_DATA_READY "Local\\Blender_DataReady"
#define EVENT_FRAME_DONE "Local\\CApp_FrameDone"

static RadialCB* g_shared_data = nullptr;

HANDLE h_data_ready = NULL;
HANDLE h_frame_done = NULL;
#endif

static void animate_stuff(RadialCB* cb);

static void update_constant_buffers() {
    if (!g_radial_cb)
        return;
    if (!g_ccsfix_cb)
        return;

    assert(g_d3d11_device);
    auto device = g_d3d11_device;
    static ID3D11DeviceContext* ctx;
    device->GetImmediateContext(&ctx);
    assert(ctx);

#if BLENDOR_SYNC
    WaitForSingleObject(h_data_ready, INFINITE);
    CopyMemory(&g_radial_cb_data, g_shared_data, sizeof(RadialCB));
#endif

    // update contrast stuff
    {
        ctx->UpdateSubresource(g_ccsfix_cb, 0, nullptr, &g_ccsfix_cb_data, 0, 0);
    }

    // update radial stuff
    {
#if !BLENDOR_SYNC
        float timer = (float)SwordColours::deathblowTimer;
        if (!g_preview_flag) {
#if 0
            g_radial_cb_data.kStrengthPx      = (float)timer * 3.0f;
            g_radial_cb_data.kChroma          = glm::smoothstep(0.0f, 50.0f, timer) * 1.4f;
            g_radial_cb_data.kTime            = (float)ImGui::GetTime();
            // TODO(deep): thread safe finish bonus, cant access HRPc stuff here ;_;
            g_radial_cb_data.kSpeedLinesScale = timer > 0.0f ? 1.0f : 0.0f;
            g_radial_cb_data.kSpeedLinesColor = SwordColours::current_s_word_color;
#endif
            animate_stuff(&g_radial_cb_data);
        }
#endif
        ctx->UpdateSubresource(g_radial_cb, 0, nullptr, &g_radial_cb_data, 0, 0);
    }

    // Bind contrast fix to PS slot b1
    ctx->PSSetConstantBuffers(1, 1, &g_ccsfix_cb);
    // Bind radial stuff to PS slot b2
    ctx->PSSetConstantBuffers(2, 1, &g_radial_cb);

#if BLENDOR_SYNC
    SetEvent(h_frame_done);
#endif
}

static void ensure_constant_buffers_exist() {
    if (g_radial_cb && g_ccsfix_cb) {
        update_constant_buffers();
        return;
    }
    assert(g_d3d11_device);
    auto device = g_d3d11_device;

    int update = 0;
    // Contrast CB
    {
        D3D11_BUFFER_DESC desc;
        ZeroMemory(&desc, sizeof(desc));
        desc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
        desc.Usage          = D3D11_USAGE_DEFAULT;
        desc.ByteWidth      = sizeof(ContrastCB);
        desc.CPUAccessFlags = 0;
        desc.MiscFlags      = 0;
        if (SUCCEEDED(device->CreateBuffer(&desc, nullptr, &g_ccsfix_cb))) {
            update += 1;
        }
    }
    // Radial CB
    {
        D3D11_BUFFER_DESC desc;
        ZeroMemory(&desc, sizeof(desc));
        desc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
        desc.Usage          = D3D11_USAGE_DEFAULT;
        desc.ByteWidth      = sizeof(RadialCB);
        desc.CPUAccessFlags = 0;
        desc.MiscFlags      = 0;
        if (SUCCEEDED(device->CreateBuffer(&desc, nullptr, &g_radial_cb))) {
            update += 1;
        }
    }

    if (update == 2) {
        update_constant_buffers();
    }
}

static void update_shader_constants() {
    ensure_constant_buffers_exist();
}

static void __stdcall ps_set_shader_ours(ID3D11DeviceContext* ctx, ID3D11PixelShader* ps, ID3D11ClassInstance* const* cl, UINT cls) {
    if (g_mod_enabled->value() && (ps == g_paper_cc_pixel_shader_game)) {
        ctx->PSSetShader(g_paper_cc_pixel_shader_ours, cl, cls);
        update_shader_constants();
        return;
    }
    ctx->PSSetShader(ps, cl, cls);
}

// clang-format off
static uintptr_t set_ps_shader_jmp_back{NULL};
naked void set_ps_shader_detour() {
    __asm {
        call ps_set_shader_ours
    originalCode:
        mov eax, 8h
        jmp dword ptr [set_ps_shader_jmp_back]
    }
}
// clang-format on

static int recreate_shader(ID3D11Device* device, CompileTimeShaderDefines& defs) {

    std::vector<char> buffer = {};
    buffer.reserve(32768);
    int samples = defs.radial_blur_samples;
    if (samples < 4)
        samples = 4;
    if (samples > 64)
        samples = 64;

    // grabbing this one just in case since d3d11 hook one can still be uninitialized :shrug:
    if (!g_d3d11_device) {
        g_d3d11_device = device;
    }

    // NOLINTBEGIN
    // dude idc
    sprintf(buffer.data(), replace_test, (int)defs.disable_cc, (int)defs.contrast_fix, (int)defs.radial_blur_fix, defs.radial_blur_samples,
        (int)defs.radial_blur_speed_lines, defs.radial_blur_speed_lines_factor);
    // NOLINTEND

    ID3DBlob* pBlob      = nullptr;
    ID3DBlob* pErrorBlob = nullptr;

    HRESULT hr = D3DCompile(buffer.data(), ::strlen(buffer.data()), nullptr, nullptr, nullptr, "main", "ps_5_0", 0, 0, &pBlob, &pErrorBlob);
    if (FAILED(hr)) {
        if (pErrorBlob) {
            OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
            pErrorBlob->Release();
        }
        return -1;
    }

    if (g_paper_cc_pixel_shader_ours) {
        g_paper_cc_pixel_shader_ours->Release();
    }
    HRESULT ret = device->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &g_paper_cc_pixel_shader_ours);
    if (FAILED(ret)) {
        spdlog::error("[ShaderEdit]: Failed to create our replacement shader!");
        return -1;
    }

    return 1;
}

std::optional<std::string> ShaderEdit::on_initialize() {
    static bool initialized = false;
    if (initialized) {
        return Mod::on_initialize();
    }

#if BLENDOR_SYNC
    // HANDLE hmap_file = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, SHM_NAME);
    HANDLE hmap_file = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, SHM_SIZE, SHM_NAME);
    if (!hmap_file) {
        auto error = GetLastError();
        return "Failed to CreateFileMappingA";
    }

    auto value    = FILE_MAP_ALL_ACCESS;
    g_shared_data = (RadialCB*)MapViewOfFile(hmap_file, FILE_MAP_ALL_ACCESS, 0, 0, SHM_SIZE);
    if (!g_shared_data) {
        CloseHandle(hmap_file);
        return "Failed to MapViewOfFile";
    }

    h_data_ready = CreateEventA(NULL, FALSE, FALSE, EVENT_DATA_READY);
    h_frame_done = CreateEventA(NULL, FALSE, FALSE, EVENT_FRAME_DONE);

    if (!h_data_ready || !h_frame_done) {
        UnmapViewOfFile(g_shared_data);
        CloseHandle(hmap_file);
        return "Failed to CreateEvent";
    }
#endif
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

void ShaderEdit::on_config_load(const utility::Config& cfg) {
    g_ccsfix_cb_data.kContrast.r     = cfg.get<float>("shaderContrastR").value_or(1.09f);
    g_ccsfix_cb_data.kContrast.g     = cfg.get<float>("shaderContrastG").value_or(1.09f);
    g_ccsfix_cb_data.kContrast.b     = cfg.get<float>("shaderContrastB").value_or(1.09f);
    g_ccsfix_cb_data.kContrast.w     = cfg.get<float>("shaderSaturation").value_or(1.0f);
    g_ccsfix_cb_data.kSMHDarkening.r = cfg.get<float>("shadowDarkening").value_or(1.0f);
    g_ccsfix_cb_data.kSMHDarkening.g = cfg.get<float>("midtoneDarkening").value_or(1.0f);
    g_ccsfix_cb_data.kSMHDarkening.b = cfg.get<float>("highlightDarkening").value_or(1.0f);

    g_comptime_shader_defs.contrast_fix                   = cfg.get<bool>("shaderContrastFixEnabled").value_or(true);
    g_comptime_shader_defs.disable_cc                     = cfg.get<bool>("shaderColorCorectionDisabled").value_or(false);
    g_comptime_shader_defs.radial_blur_fix                = cfg.get<bool>("shaderRadialBlurFix").value_or(true);
    g_comptime_shader_defs.radial_blur_samples            = cfg.get<int>("shaderRadialBlurSamples").value_or(12);
    g_comptime_shader_defs.radial_blur_speed_lines        = cfg.get<bool>("shaderRadialBlurAnimeSpeedLines").value_or(true);
    g_comptime_shader_defs.radial_blur_speed_lines_factor = cfg.get<float>("shaderRadialBlurSpeedLinesFactor").value_or(0.18f);
    g_comptime_shader_defs.radial_blur_strength_pixels    = cfg.get<float>("shaderRadialBlurStrengthPixels").value_or(160.0f);

    recreate_shader(g_d3d11_device, g_comptime_shader_defs);

    g_mod_enabled->config_load(cfg);
}

void ShaderEdit::on_config_save(utility::Config& cfg) {
    cfg.set<float>("shaderContrastR", g_ccsfix_cb_data.kContrast.r);
    cfg.set<float>("shaderContrastG", g_ccsfix_cb_data.kContrast.g);
    cfg.set<float>("shaderContrastB", g_ccsfix_cb_data.kContrast.b);
    cfg.set<float>("shaderSaturation", g_ccsfix_cb_data.kContrast.w);
    cfg.set<float>("shadowDarkening", g_ccsfix_cb_data.kSMHDarkening.r);
    cfg.set<float>("midtoneDarkening", g_ccsfix_cb_data.kSMHDarkening.g);
    cfg.set<float>("highlightDarkening", g_ccsfix_cb_data.kSMHDarkening.b);

    cfg.set<bool>("shaderContrastFixEnabled", g_comptime_shader_defs.contrast_fix);
    cfg.set<bool>("shaderColorCorectionDisabled", g_comptime_shader_defs.disable_cc);
    cfg.set<bool>("shaderRadialBlurFix", g_comptime_shader_defs.radial_blur_fix);
    cfg.set<int>("shaderRadialBlurSamples", g_comptime_shader_defs.radial_blur_samples);
    cfg.set<bool>("shaderRadialBlurAnimeSpeedLines", g_comptime_shader_defs.radial_blur_speed_lines);
    cfg.set<float>("shaderRadialBlurAnimeSpeedLinesFactor", g_comptime_shader_defs.radial_blur_speed_lines_factor);
    cfg.set<float>("shaderRadialBlurStrengthPixels", g_comptime_shader_defs.radial_blur_strength_pixels);

    g_mod_enabled->config_save(cfg);
}

#if 0
namespace blender {

    enum class MapRangeInterpolation {
        Linear,
        SteppedLinear,
        SmoothStep,
        SmootherStep
    };

    struct MapRangeNode {
        float value        = 0.0f;
        float from_min     = 0.0f;
        float from_max     = 1.0f;
        float to_min       = 0.0f;
        float to_max       = 1.0f;
        int   steps        = 4;
        MapRangeInterpolation mode = MapRangeInterpolation::Linear;
        bool  clamp        = false;

        float operator()() const noexcept {
            float t = (value - from_min) / (from_max - from_min);
            if (clamp) t = std::clamp(t, 0.0f, 1.0f);

            switch (mode) {
            case MapRangeInterpolation::Linear:        break;
            case MapRangeInterpolation::SteppedLinear: t = std::floor(t * steps) / steps; break;
            case MapRangeInterpolation::SmoothStep:    t = t*t*(3.0f - 2.0f*t); break;
            case MapRangeInterpolation::SmootherStep:  t = t*t*t*(t*(t*6.0f - 15.0f) + 10.0f); break;
            }

            return to_min + t * (to_max - to_min);
        }
    };

} // namespace blender
#endif

#include "fw-imgui/RadialBlurAnim.cpp"
static constexpr RadialAnimationData rdata;

static bool animating = false;
static void animate_stuff(RadialCB* cb) {
    static int anim_frame = 0;

    int timer = SwordColours::deathblowTimer;
    if (timer > 0) {
        animating = true;
    }

    if (animating) {
        int frame = anim_frame;
        // int frame = timer;
        if (frame >= rdata.k_frames) {
            animating = false;
            memset(cb, 0, sizeof(RadialCB));
            anim_frame = 0;
            return;
        }
        // append_ui_log(fmt::format("timer: {}; anim_frames: = {}", timer, frame));
        cb->kStrengthPx      = rdata.k_strengths[frame] * 160.0f;
        cb->kInner           = 0.02f;
        cb->kOuter           = rdata.k_outers[frame];
        cb->kFalloff         = rdata.k_falloffs[frame];
        cb->kSpeedLinesScale = rdata.k_speed_lines[frame];
        cb->kChroma          = rdata.k_chromas[frame];
        cb->kTime            = (float)ImGui::GetTime();
        cb->kMix             = 1.0f;
        cb->kSpeedLinesColor = SwordColours::current_s_word_color;
        anim_frame += 1;
    }
}

void ImGoo_CCheckbox(const char* label, bool* v) {
    if (ImGui::Checkbox(label, v)) {
        recreate_shader(g_d3d11_device, g_comptime_shader_defs);
    }
}

void ImGoo_CInt(const char* label, int* v) {
    if (ImGui::SliderInt(label, v, 4, 64)) {
        recreate_shader(g_d3d11_device, g_comptime_shader_defs);
    }
}

void ImGoo_CFloat(const char* label, float* v) {
    if (ImGui::SliderFloat(label, v, 0.0f, 1.8f)) {
        recreate_shader(g_d3d11_device, g_comptime_shader_defs);
    }
}

void ShaderEdit::on_draw_ui() {
    g_mod_enabled->draw("Enable the thing?");

    if (!g_mod_enabled->value()) {
        return;
    }

    if (ImGui::TreeNode("Compile time shader settings")) {

        ImGoo_CCheckbox("Diable Color Correction?", &g_comptime_shader_defs.disable_cc);
        ImGoo_CCheckbox("Enable contrast fix?", &g_comptime_shader_defs.contrast_fix);
        ImGoo_CCheckbox("Enable radial blur fix?", &g_comptime_shader_defs.radial_blur_fix);
        if (g_comptime_shader_defs.radial_blur_fix) {
            ImGoo_CInt("Number of samples [4-64]", &g_comptime_shader_defs.radial_blur_samples);
        }

        ImGoo_CCheckbox("Enable speed lines?", &g_comptime_shader_defs.radial_blur_speed_lines);
        if (g_comptime_shader_defs.radial_blur_speed_lines) {
            ImGoo_CFloat("Speed lines factor", &g_comptime_shader_defs.radial_blur_speed_lines_factor);
        }
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Contrast settings")) {
        ImGui::SliderFloat3("Contrast", (float*)&g_ccsfix_cb_data.kContrast, 0.0f, 3.0f);
        ImGui::SliderFloat3("Shadows\\Midtones\\Highlights", (float*)&g_ccsfix_cb_data.kSMHDarkening, 0.0f, 3.0f);
        ImGui::SliderFloat("Saturation", (float*)&g_ccsfix_cb_data.kContrast.w, 0.0f, 3.0f);
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Radial Blur Settings")) {

        ImGui::Checkbox("Preview mode", &g_preview_flag);
        if (ImGui::SliderFloat("strength px: ", &g_radial_cb_data.kStrengthPx, 0.0f, 300.0f)) {
            g_comptime_shader_defs.radial_blur_strength_pixels = g_radial_cb_data.kStrengthPx;
        }
        ImGui::SliderFloat("Mix (0=Off,1=Full)", &g_radial_cb_data.kMix, 0.0f, 1.0f, "%.2f");
        ImGui::SliderFloat("Inner radius", &g_radial_cb_data.kInner, 0.0f, 1.0f, "%.2f");
        ImGui::SliderFloat("Outer radius", &g_radial_cb_data.kOuter, 0.0f, 1.0f, "%.2f");
        ImGui::SliderFloat("Falloff (shape)", &g_radial_cb_data.kFalloff, 0.1f, 8.0f, "%.2f");
        ImGui::SliderFloat("Twist at edge (deg)", &g_radial_cb_data.kTwistDeg, -360.0f, 360.0f, "%.1f");
        ImGui::SliderFloat("Chromatic fringing", &g_radial_cb_data.kChroma, 0.0f, 0.5f, "%.2f");
        if (g_preview_flag) {
            ImGui::SliderFloat("Anime speed lines", &g_radial_cb_data.kSpeedLinesScale, 0.0f, 1.0f);
            ImGui::SliderFloat("Anime speed lines time", &g_radial_cb_data.kTime, 1.0f, 130.0f);
            ImGui::ColorEdit3("Anime speed lines color", (float*)&g_radial_cb_data.kSpeedLinesColor);
        }

        ImGui::TreePop();
    }
}
