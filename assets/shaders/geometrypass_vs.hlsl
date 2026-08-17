cbuffer Transform : register(b0)
{
    matrix Model;
    matrix View;
    matrix Projection;

    matrix ModelView;
    matrix InvModelView;
};

struct VS_INPUT
{
    float3 iPos         : ATTRIB0;
    float3 iNormal      : ATTRIB1;
    float3 iTangent     : ATTRIB2;
    float3 iBitangent   : ATTRIB3;
    float2 iUV          : ATTRIB4;
    float4 iColor       : ATTRIB5;
};

struct VS_OUTPUT
{
    float4 vPos         : SV_POSITION;
    float3 vViewPos     : POSITION;
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
    float4 viewPos = mul(worldPos, View);

    float3 N = normalize(mul(input.iNormal, (float3x3)InvModelView));
    float3 T = normalize(mul(input.iTangent, (float3x3)ModelView));
	
    o.vPos = mul(viewPos, Projection);
    o.vViewPos = viewPos.xyz;
    o.vNormal = N;
    o.vTangent = T;
    o.vBitangent = input.iBitangent;
    o.vUV = input.iUV;
    o.vColor = input.iColor;

    return o;
}