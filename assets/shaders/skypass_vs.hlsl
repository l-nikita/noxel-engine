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
    o.vUV = input.iPos;

    pos = mul(pos, View);
    pos = mul(pos, Projection);
    o.vPos = pos.xyww;

    return o;
}