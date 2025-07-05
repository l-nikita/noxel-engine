FragmentShader
[
	cbuffer ColorBuffer : register(b1)
	{
		float4 color;
	};

	float4 main() : SV_TARGET
	{
		return color;
	}
]

VertexShader
[
	cbuffer Matrices : register(b0)
	{
		matrix model;
		matrix view;
		matrix projection;
	}

	struct VS_INPUT
	{
		float3 aPosition : POSITION;
	};

	struct VS_OUTPUT
	{
		float4 Position : SV_POSITION;
	};

	VS_OUTPUT main(VS_INPUT input)
	{
		VS_OUTPUT output;

		float4 worldPos = mul(float4(input.aPosition, 1.0f), model);
		float4 viewPos = mul(worldPos, view);
		output.Position = mul(viewPos, projection);

		return output;
	}
]