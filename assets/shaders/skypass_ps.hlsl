//----------------------------------------
TextureCube g_tSkyTexture  	    : register(t0);

//----------------------------------------
SamplerState g_sSampler         : register(s0);

//----------------------------------------
struct PS_INPUT
{
    float4 vPos     : SV_POSITION;
    float3 vUV      : TEXCOORD0;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    return g_tSkyTexture.Sample(g_sSampler, input.vUV);
}