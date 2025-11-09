//----------------------------------------
TextureCube SkyTexture  	: register(t0);

//----------------------------------------
SamplerState Samp       : register(s0);

//----------------------------------------
struct PS_INPUT
{
    float4 vPos     : SV_POSITION;
    float3 vUV      : TEXCOORD0;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    return SkyTexture.Sample(Samp, input.vUV);
}