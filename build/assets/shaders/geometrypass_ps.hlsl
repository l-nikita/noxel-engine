Texture2D Texture0 : register(t0); // color texture
Texture2D Texture1 : register(t1); // normal texture
SamplerState Samp : register(s0);
	
struct PS_INPUT
{
    float4 vPos         : SV_POSITION;
    float3 vWorldPos    : POSITION;
    float3 vNormal      : NORMAL;
    float3 vTangent     : TANGENT;
    float3 vBitangent   : BINORMAL;
    float2 vUV          : TEXCOORD0;
    float4 vColor       : COLOR;
};
	
struct PS_OUTPUT
{
    float4 pColor       : SV_Target0;
    float4 pNormal      : SV_Target1;
    float4 pPosition    : SV_Target2;
};
	
PS_OUTPUT main(PS_INPUT input) : SV_TARGET
{
    PS_OUTPUT o;
    
    float4 color = Texture0.Sample(Samp, input.vUV) * input.vColor;
    float3 normalTex = Texture1.Sample(Samp, input.vUV).xyz * 2.0 - 1.0;
    
    float3 N = normalize(input.vNormal);
    float3 T = normalize(input.vTangent - dot(input.vTangent, N) * N);
    float3 B = cross(N, T);
    float3x3 TBN = float3x3(T, B, N);
    float3 normal = normalize(mul(normalTex, TBN));
    
    o.pColor = color;
    o.pNormal = float4(normal, 1.0);
    o.pPosition = float4(input.vWorldPos, 1.0);

    return o;
}