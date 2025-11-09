//----------------------------------------
#include "lightning.hlsl"

//----------------------------------------
Texture2D gColor : register(t0);
Texture2D gNormal : register(t1);
Texture2D gPosition : register(t2);

Texture2D ShadowMaps0 : register(t3);
Texture2D ShadowMaps1 : register(t4);
Texture2D ShadowMaps2 : register(t5);
Texture2D ShadowMaps3 : register(t6);

//----------------------------------------
SamplerState Samp : register(s0);
SamplerComparisonState ShadowSampler : register(s1);

//----------------------------------------
cbuffer LightsData : register(b0)
{
    Light Lights[MAX_LIGHTS];
    float3 ViewPos;
};

//----------------------------------------
struct PS_INPUT
{
    float4 vPos : SV_POSITION;
    float2 vUV : TEXCOORD0;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    // gbuffer
    float3 color = gColor.Sample(Samp, input.vUV).rgb;
    float3 normal = gNormal.Sample(Samp, input.vUV).rgb;
    float3 worldPos = gPosition.Sample(Samp, input.vUV).rgb;

    float3 toView = normalize(ViewPos - worldPos); // view direction (to camera)

    // ambient
    float3 ambient = CalcAmbient();

    //--------------
    float3 result = color * ambient;
    
    // lighting
    [unroll]
    for (int i = 0; i < MAX_LIGHTS; i++)
    {
        int lightType = Lights[i].Type;
        float lightRadius = Lights[i].Radius;
        float lightCutoff = Lights[i].Cutoff;
        float3 lightPos = Lights[i].Pos;
        float3 lightColor = Lights[i].Color;
        float3 lightDir = Lights[i].Dir;
        matrix lightView = Lights[i].View;
        matrix lightProjection = Lights[i].Projection;
        
        //----------------------------------------
        float3 toLight = normalize(lightPos - worldPos);
        float distance = length(lightPos - worldPos);
        float attenuation = 1.0;
		float shadow = 1.0;
        
        //----------------------------------------
        if (lightType == LIGHT_POINT)
        {
            toLight = normalize(lightPos - worldPos);
            attenuation = saturate(1.0 - distance / lightRadius);
        }
        else if (lightType == LIGHT_SPOT)
        {
            toLight = normalize(lightPos - worldPos);
            float att = saturate(1.0 - distance / lightRadius);

            float theta = dot(toLight, normalize(-lightDir));
            float outerCutoff = lightCutoff;
            float innerCutoff = outerCutoff + 0.1;
			
            float spotEffect = smoothstep(outerCutoff, innerCutoff, theta);
            attenuation = att * spotEffect;
            
            int index = Lights[i].ShadowMapIndex;

            if (index >= 0)
            {
                if (index == 0)
                    shadow = CalcSpotShadows(worldPos, normal, toLight, lightView, lightProjection, ShadowMaps0, ShadowSampler);
                else if (index == 1)
                    shadow = CalcSpotShadows(worldPos, normal, toLight, lightView, lightProjection, ShadowMaps1, ShadowSampler);
                else if (index == 2)
                    shadow = CalcSpotShadows(worldPos, normal, toLight, lightView, lightProjection, ShadowMaps2, ShadowSampler);
                else if (index == 3)
                    shadow = CalcSpotShadows(worldPos, normal, toLight, lightView, lightProjection, ShadowMaps3, ShadowSampler);
            }

        }
        else if (lightType == LIGHT_DIRECTIONAL)
        {
            toLight = normalize(-lightDir);
            attenuation = 1.0;
        }

        //----------------------------------------
        // Diffuse
        float diff = max(dot(normal, toLight), 0.0);
        float3 diffuse = diff * color * lightColor * attenuation * shadow;

        //----------------------------------------
        // specular (blinn phong)
        float3 H = normalize(toLight + toView);
        float specAngle = max(dot(normal, H), 0.0);
        
        float ks = 1.5; // коэффициент блеска
        float shininess = 32.0; // ширина блика
        float3 specularColor = ks * pow(specAngle, shininess) * lightColor * attenuation * shadow;

        result += diffuse + specularColor;
    }
    
    return float4(result, 1.0);
}