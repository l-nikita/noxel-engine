cbuffer Transform : register(b0)
{
    matrix Model;
    matrix View;
    matrix Projection;
};

struct VS_INPUT
{
    float3 iPos     : POSITION;
};

struct VS_OUTPUT
{
    float4 vPos     : SV_POSITION;
    float3 vUV      : TEXCOORD0;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT o;

    float4 pos = float4(input.iPos, 1.0f);

    float4x4 viewNoTrans = View;
    viewNoTrans._41 = 0;
    viewNoTrans._42 = 0;
    viewNoTrans._43 = 0;

    o.vUV = input.iPos;

    pos = mul(pos, viewNoTrans);
    pos = mul(pos, Projection);
    o.vPos = pos.xyww;

    
    return o;
}