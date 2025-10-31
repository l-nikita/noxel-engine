Texture2D gColor : register(t0);
Texture2D gNormal : register(t1);
Texture2D gPosition : register(t2);
SamplerState Samp : register(s0);

#define NUM_LIGHTS 4

#define LIGHT_POINT 0
#define LIGHT_SPOT 1
#define LIGHT_DIRECTIONAL 2

struct Light
{
    int Type;
    float pad0, pad1, pad2;
    float3 Pos;
    float Radius;
    float3 Color;
    float Cutoff;
    float3 Dir;
    float pad3;
};

cbuffer LightsData : register(b0)
{
    Light Lights[NUM_LIGHTS];
    float3 ViewPos;
};

struct PS_INPUT
{
	float4 iPos : SV_POSITION;
	float2 iUV : TEXCOORD0;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    // gbuffer
    float3 color = gColor.Sample(Samp, input.iUV).rgb;
    float3 normal = gNormal.Sample(Samp, input.iUV).rgb;
    float3 worldPos = gPosition.Sample(Samp, input.iUV).rgb;

    float3 V = normalize(ViewPos - worldPos); // view direction (to camera)

    // ambient
    float3 ambient = 0.05;

    //--------------
    float3 result = color * ambient;
    
    // lighting
    [unroll]
    for (int i = 0; i < NUM_LIGHTS; i++)
    {
        int lightType = Lights[i].Type;
        float lightRadius = Lights[i].Radius;
        float lightCutoff = Lights[i].Cutoff;
        float3 lightPos = Lights[i].Pos;
        float3 lightColor = Lights[i].Color;
        float3 lightDir = Lights[i].Dir;
        
        // --- Light vector ---
        float3 L = normalize(lightPos - worldPos);
        float distance = length(lightPos - worldPos);
        float attenuation = 1.0;
        
        if (lightType == LIGHT_POINT)
        {
            L = normalize(lightPos - worldPos);
            attenuation = saturate(1.0 - distance / lightRadius);
        }
        else if (lightType == LIGHT_SPOT)
        {
            L = normalize(lightPos - worldPos);
            float att = saturate(1.0 - distance / lightRadius);

            float spotEffect = dot(L, normalize(-lightDir));
            spotEffect = smoothstep(lightCutoff, 1.0, spotEffect);
            attenuation = att * spotEffect;
        }
        else if (lightType == LIGHT_DIRECTIONAL)
        {
            L = normalize(-lightDir);
            attenuation = 1.0;
        }

        // --- Diffuse ---
        float diff = max(dot(normal, L), 0.0);
        float3 diffuse = diff * color * lightColor * attenuation;

        // --- Specular (Blinn-Phong) ---
        float3 H = normalize(L + V);
        float specAngle = max(dot(normal, H), 0.0);
        
        float ks = 1.5; // коэффициент блеска
        float shininess = 32.0; // ширина блика
        float3 specularColor = ks * pow(specAngle, shininess) * lightColor * attenuation;

        result += diffuse + specularColor;
    }
    
    return float4(result, 1.0);
}