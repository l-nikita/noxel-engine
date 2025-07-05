FragmentShader
[
	Texture2D tex : register(t0);
	SamplerState smp : register(s0);
	struct PSInput {
		float4 pos : SV_POSITION;
		float4 col : COLOR;
		float2 uv : TEXCOORD0;
	};
	float4 main(PSInput input) : SV_TARGET {
		return tex.Sample(smp, input.uv);
	}
]

VertexShader
[
	cbuffer ConstantBuffer : register(b0) {
		float4x4  model;
		float4x4  view;
		float4x4  projection;
	};

	struct VSInput {
		float3 pos : POSITION;
		float4 col : COLOR;
		float2 uv  : TEXCOORD0;
	};

	struct VSOutput {
		float4 pos : SV_POSITION;
		float4 col : COLOR;
		float2 uv  : TEXCOORD0;
	};

	VSOutput main(VSInput input) {
		VSOutput output;
		
		float4 worldPos = mul(model, float4(input.pos, 1.0f));
		float4 viewPos = mul(view, worldPos);
		output.pos = mul(projection, viewPos);
		
		output.col = input.col;
		output.uv = input.uv;
		return output;
	}
]