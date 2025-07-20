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
    col.rgb = (col.rgb - 0.5f) * 1.09f + 0.5f;
    o0.xyzw = v0.xyzw * col.xyzw; //r0.xyzw;
    return;
}