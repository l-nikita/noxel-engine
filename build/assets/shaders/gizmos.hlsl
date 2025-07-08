FragmentShader
[
	cbuffer ColorBuffer : register(b0)
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
	cbuffer ConstantBuffer : register(b0) 
	{
		matrix model;
		matrix view;
		matrix projection;
	};

	struct VSInput 
	{
		float3 pos : POSITION;
		float4 col : COLOR;
		float2 uv  : TEXCOORD0;
	};

	struct VSOutput 
	{
		float4 pos : SV_POSITION;
		float4 col : COLOR;
		float2 uv  : TEXCOORD0;
	};

	VSOutput main(VSInput input) 
	{
		VSOutput output;
		
		output.pos = mul(mul(mul(float4(input.pos, 1.0f), model), view), projection);
		
		output.col = input.col;
		output.uv = input.uv;
		return output;
	}
]