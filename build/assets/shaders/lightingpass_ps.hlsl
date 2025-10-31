Texture2D gColor : register(t0);
Texture2D gNormal : register(t1);
Texture2D gPosition : register(t2);
SamplerState Samp : register(s0);

cbuffer LightData : register(b0)
{
	float3 LightPos;
	float pad0;
	float3 LightColor;
	float pad1;
	float3 CameraPos;
	float pad2;
};

struct PS_INPUT
{
	float4 iPos : SV_POSITION;
	float2 iUV : TEXCOORD0;
};

float4 main(PS_INPUT input) : SV_TARGET
{
	float3 objectColor = gColor.Sample(Samp, input.iUV).rgb;
	float3 normal = normalize(gNormal.Sample(Samp, input.iUV).rgb * 2.0 - 1.0);
	float3 worldPos = gPosition.Sample(Samp, input.iUV).rgb;

	float3 lightVec = LightPos - worldPos;
	float distance = length(lightVec);
	float3 lightDir = normalize(lightVec);

	float attenuation = 1.0 / (distance * distance + 1.0);

	float diff = max(dot(normal, lightDir), 0.0);
	float3 diffuse = diff * LightColor * attenuation;

	float3 viewDir = normalize(CameraPos - worldPos);
	float3 halfDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfDir), 0.0), 32);
	float3 specular = 0.2 * spec * LightColor * attenuation;

	float3 ambient = 0.05 * objectColor;

	float3 lighting = ambient + diffuse + specular;
	float3 result = objectColor * lighting;

	return float4(result, 1.0);
}