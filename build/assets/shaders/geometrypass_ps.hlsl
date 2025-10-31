Texture2D Texture0 : register(t0);
Texture2D Texture1 : register(t1); // Normal
SamplerState Samp : register(s0);
	
struct PS_INPUT
{
    float4 iPos : SV_POSITION;
    float3 iWorldPos : POSITION;
    float3 iNormal : NORMAL;
    float3 iTangent : TANGENT;
    float3 iBitangent : BINORMAL;
    float2 iUV : TEXCOORD;
    float4 iColor : COLOR;
};
	
struct PS_OUTPUT
{
    float4 oColor : SV_Target0;
    float4 oNormal : SV_Target1;
    float4 oPosition : SV_Target2;
};
	
PS_OUTPUT main(PS_INPUT input) : SV_TARGET
{
    PS_OUTPUT o;
    
    float4 color = Texture0.Sample(Samp, input.iUV) * input.iColor;
    o.oColor = color;

    float3 nTex = Texture1.Sample(Samp, input.iUV).xyz * 2.0 - 1.0;
    float3x3 TBN = float3x3(input.iTangent, input.iBitangent, input.iNormal);

    float3 worldNormal = normalize(mul(nTex, TBN));
    o.oNormal = float4(worldNormal * 0.5 + 0.5, 1.0);
    
    o.oPosition = float4(input.iWorldPos, 1.0);

    return o;
}