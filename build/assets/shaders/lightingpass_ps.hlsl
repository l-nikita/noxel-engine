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
    // --- Read G-buffer ---
    float3 albedo = gColor.Sample(Samp, input.iUV).rgb;
    float3 normal = gNormal.Sample(Samp, input.iUV).rgb;
    float3 worldPos = gPosition.Sample(Samp, input.iUV).rgb;

    // --- Lighting setup ---
    float3 L = normalize(LightPos - worldPos); // light direction (to light)
    float3 V = normalize(CameraPos - worldPos); // view direction (to camera)

    // --- Attenuation ---
    float distance = length(LightPos - worldPos);
    float attenuation = 1.0 / (distance * distance * 0.1 + 1.0);

    // --- Ambient term ---
    float3 ambient = 0 * albedo;

    // --- Diffuse term (Lambert) ---
    float diff = max(dot(normal, L), 0.0);
    float3 diffuse = diff * albedo * LightColor * attenuation;

    // --- Specular term (Phong reflection model) ---
    float3 R = reflect(-L, normal); // reflection vector
    float spec = pow(max(dot(V, R), 0.0), 64.0);
    float3 specular = 1.0 * spec * LightColor * attenuation;

    // --- Final lighting ---
    float3 result = ambient + diffuse + specular;

    return float4(result, 1.0);
}