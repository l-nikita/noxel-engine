struct VS_OUTPUT
{
	float4 oPos : SV_POSITION;
	float2 oUV : TEXCOORD0;
};

VS_OUTPUT main(uint id : SV_VertexID)
{
	VS_OUTPUT o;

	float2 pos[3] =
	{
		float2(-1.0f, -1.0f),
		float2(-1.0f, 3.0f),
		float2(3.0f, -1.0f)
	};

	o.oPos = float4(pos[id], 0.0f, 1.0f);
	o.oUV = float2((pos[id].x + 1.0f) * 0.5f, 1.0f - (pos[id].y + 1.0f) * 0.5f);

	return o;
}