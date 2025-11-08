Texture2D gColor : register(t0);
Texture2D gNormal : register(t1);
Texture2D gPosition : register(t2);
SamplerState Samp : register(s0);

Texture2D ShadowMap : register(t3);
SamplerComparisonState ShadowSampler : register(s1);

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

cbuffer LightMatrices : register(b1)
{
    matrix LightView;
    matrix LightProjection;
}

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
		float shadow = 1.0;
        
        if (lightType == LIGHT_POINT)
        {
            L = normalize(lightPos - worldPos);
            attenuation = saturate(1.0 - distance / lightRadius);
        }
        else if (lightType == LIGHT_SPOT)
        {
            L = normalize(lightPos - worldPos);
            float att = saturate(1.0 - distance / lightRadius);

			float theta = dot(L, normalize(-lightDir));
			float outerCutoff = lightCutoff;
			float innerCutoff = outerCutoff + 0.1;
			
			float spotEffect = smoothstep(outerCutoff, innerCutoff, theta);
            attenuation = att * spotEffect;

            float4 lightSpacePos = mul(float4(worldPos, 1.0), LightView);
            lightSpacePos = mul(lightSpacePos, LightProjection);
			
			lightSpacePos.xyz /= lightSpacePos.w;

			float2 shadowUV = float2(
				lightSpacePos.x * 0.5 + 0.5,
				-lightSpacePos.y * 0.5 + 0.5
			);
			float shadowDepth = lightSpacePos.z;
			
            if (shadowUV.x >= 0.0 && shadowUV.x <= 1.0 && 
                shadowUV.y >= 0.0 && shadowUV.y <= 1.0 &&
                shadowDepth >= 0.0 && shadowDepth <= 1.0)
            {
				//float bias = 0.0001;
				float bias = lerp(0.00015, 0.0003, saturate(1 - dot(normal, L)));
				uint shadowWidth, shadowHeight;
				ShadowMap.GetDimensions(shadowWidth, shadowHeight);
				
				float2 texelSize = 1.0 / float2(shadowWidth, shadowHeight);
				float shadowSum = 0.0;
				
				const int range = 1; // 1 = 3x3, 2 = 5x5

				for (int x = -range; x <= range; ++x)
				{
					for (int y = -range; y <= range; ++y)
					{
						float2 offset = float2(x, y) * texelSize;
						shadowSum += ShadowMap.SampleCmpLevelZero(ShadowSampler, shadowUV + offset, shadowDepth - bias);
					}
				}
				
				shadow = shadowSum / pow((range * 2 + 1), 2);
            }
        }
        else if (lightType == LIGHT_DIRECTIONAL)
        {
            L = normalize(-lightDir);
            attenuation = 1.0;
        }

        // --- Diffuse ---
        float diff = max(dot(normal, L), 0.0);
        float3 diffuse = diff * color * lightColor * attenuation * shadow;

        // --- Specular (Blinn-Phong) ---
        float3 H = normalize(L + V);
        float specAngle = max(dot(normal, H), 0.0);
        
        float ks = 1.5; // коэффициент блеска
        float shininess = 32.0; // ширина блика
        float3 specularColor = ks * pow(specAngle, shininess) * lightColor * attenuation * shadow;

        result += diffuse + specularColor;
    }
    
    return float4(result, 1.0);
}