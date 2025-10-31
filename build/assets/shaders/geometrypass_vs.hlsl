cbuffer Transform : register(b0)
{
    matrix Model;
    matrix View;
    matrix Projection;
};

struct VS_INPUT
{
    float3 iPos : POSITION;
    float3 iNormal : NORMAL;
    float3 iTangent : TANGENT;
    float3 iBitangent : BINORMAL;
    float2 iUV : TEXCOORD;
    float4 iColor : COLOR;
};

struct VS_OUTPUT
{
    float4 oPos : SV_POSITION;
    float3 oNormal : NORMAL;
    float3 oTangent : TANGENT;
    float3 oBitangent : BINORMAL;
    float3 oWorldPos : POSITION;
    float2 oUV : TEXCOORD0;
    float4 oColor : COLOR;
};

float3x3 inverse3x3(float3x3 m)
{
    float3 a = m[0];
    float3 b = m[1];
    float3 c = m[2];
    float3 r0 = cross(b, c);
    float3 r1 = cross(c, a);
    float3 r2 = cross(a, b);
    float invDet = 1.0 / dot(r2, c);
    return float3x3(r0 * invDet, r1 * invDet, r2 * invDet);
}

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT o;

    float4 worldPos = mul(float4(input.iPos, 1.0f), Model);
    o.oWorldPos = worldPos.xyz;
    o.oPos = mul(mul(worldPos, View), Projection);

    float3x3 normalMat = (float3x3) Model;
    float3 N = normalize(mul(input.iNormal, normalMat));
	
    o.oNormal = N;
    o.oTangent = input.iTangent;
    o.oBitangent = input.iBitangent;

    o.oUV = input.iUV;
    o.oColor = input.iColor;

    return o;
}