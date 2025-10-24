PixelShader
[
	Texture2D Texture0 : register(t0);
	SamplerState Samp : register(s0);
	
	struct PS_INPUT
	{
		float4 iPos 		: SV_POSITION;
		float3 iWorldPos 	: POSITION;
		float3 iNormal 		: NORMAL;
		float2 iUV 			: TEXCOORD;
		float4 iColor      	: COLOR;
	};
	
	struct PS_OUTPUT
	{
		float4 oColor 		: SV_Target0;
		float4 oNormal 		: SV_Target1;
		float4 oPosition 	: SV_Target2;
	};
	
	PS_OUTPUT main(PS_INPUT input) : SV_TARGET 
	{
		PS_OUTPUT o;
		
		float4 color = Texture0.Sample(Samp, input.iUV);
		color = color * input.iColor;
		
		o.oColor = color;
		o.oNormal = float4(normalize(input.iNormal) * 0.5f + 0.5f, 1.0f);
		o.oPosition = float4(input.iWorldPos, 1);
		
		return o;
	}
]

VertexShader
[
	cbuffer Transform : register(b0)
	{
		matrix Model;
		matrix View;
		matrix Projection;
	};

	struct VS_INPUT 
	{
		float3 iPos			: POSITION;
		float3 iNormal		: NORMAL;
		float2 iUV			: TEXCOORD;
		float4 iColor		: COLOR;
	};

	struct VS_OUTPUT 
	{
		float4 oPos			: SV_POSITION;
		float3 oNormal		: NORMAL;
		float3 oWorldPos	: POSITION;
		float2 oUV			: TEXCOORD0;
		float4 oColor		: COLOR;
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
		float3x3 normalMat = transpose(inverse3x3((float3x3) Model));
		o.oNormal = mul(normalMat, input.iNormal);

		o.oUV = input.iUV;
		o.oColor = input.iColor;
		
		return o;
	}
]