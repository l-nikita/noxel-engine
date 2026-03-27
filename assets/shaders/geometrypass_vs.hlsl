cbuffer Transform : register(b0)
{
    matrix Model;
    matrix View;
    matrix Projection;
};

struct VS_INPUT
{
    float3 iPos         : POSITION;
    float3 iNormal      : NORMAL;
    float3 iTangent     : TANGENT;
    float3 iBitangent   : BINORMAL;
    float2 iUV          : TEXCOORD;
    float4 iColor       : COLOR;
};

struct VS_OUTPUT
{
    float4 vPos         : SV_POSITION;
    float3 vWorldPos    : POSITION;
    float3 vNormal      : NORMAL;
    float3 vTangent     : TANGENT;
    float3 vBitangent   : BINORMAL;
    float2 vUV          : TEXCOORD0;
    float4 vColor       : COLOR;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT o;

    float4 worldPos = mul(float4(input.iPos, 1.0f), Model);
    float3x3 normalMat = (float3x3)Model;
    float3 N = normalize(mul(input.iNormal, normalMat));
	
    o.vPos = mul(mul(worldPos, View), Projection);
    o.vWorldPos = worldPos.xyz;
    o.vNormal = N;
    o.vTangent = input.iTangent;
    o.vBitangent = input.iBitangent;
    o.vUV = input.iUV;
    o.vColor = input.iColor;

    return o;
}