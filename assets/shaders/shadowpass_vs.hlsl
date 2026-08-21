cbuffer Transform : register(b0)
{
    matrix Model;
    matrix View;
    matrix Projection;
};

struct VS_INPUT
{
    float3 iPos : ATTRIB0;
};

struct VS_OUTPUT
{
    float4 vPos : SV_POSITION;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT o;
	float4 worldPos = mul(float4(input.iPos, 1.0f), Model);
	o.vPos = mul(mul(worldPos, View), Projection);
    return o;
}