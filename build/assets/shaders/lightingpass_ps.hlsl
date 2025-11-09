//----------------------------------------
#include "lighting.hlsl"

#define SIMPLE_LIGHTING 0

//----------------------------------------
Texture2D gColor : register(t0);
Texture2D gNormal : register(t1);
Texture2D gPosition : register(t2);
Texture2D gSpecular : register(t3);

Texture2D ShadowMaps0 : register(t4);
Texture2D ShadowMaps1 : register(t5);
Texture2D ShadowMaps2 : register(t6);
Texture2D ShadowMaps3 : register(t7);

TextureCube Cubemap : register(t10);

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
    float3 spec = gSpecular.Sample(Samp, input.vUV);
    
    float roughness = spec.r;
    roughness = saturate(max(roughness, 0.04));
    float metalness = spec.g;
    float ao = spec.b;

    float3 toView = normalize(ViewPos - worldPos); // view direction (to camera)

    // ambient
#if SIMPLE_LIGHTING
    float3 ambient = CalcSimpleAmbient() * color * ao * 0.03;
    float3 result = ambient;
#else
    float3 result = CalcIBL(normal, toView, color, metalness, roughness, ao, Cubemap, Samp);
#endif
    
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
        
#if SIMPLE_LIGHTING
        //----------------------------------------
        // Diffuse
        float3 diffuse = CalcSimpleLightDiffuse(normal, toLight, color) * lightColor * attenuation * shadow;

        //----------------------------------------
        // specular (blinn phong)
        float3 specularColor = CalcSimpleLightSpecular(normal, toLight, toView) * lightColor * attenuation * shadow;
        
        result += diffuse + specularColor;
#else
        //-------------------- PBR Cook-Torrance --------------------
        float3 N = normalize(normal);
        float3 V = normalize(toView);
        float3 L = normalize(toLight);
        float3 H = normalize(V + L);

        float NdotL = max(dot(N, L), 0.0);
        float NdotV = max(dot(N, V), 0.0);
        float NdotH = max(dot(N, H), 0.0);
        float HdotV = max(dot(H, V), 0.0);

        // base reflectivity (dielectric = 0.04, metal = albedo)
        float3 F0 = lerp(float3(0.04, 0.04, 0.04), color, metalness);

        // microfacet terms
        float3 F = FresnelSchlick(HdotV, F0);
        float D = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);

        // cook torrance BRDF
        float3 numerator = D * G * F;
        float denominator = 4.0 * NdotV * NdotL + 0.001;
        float3 specular = numerator / denominator;

        // energy conservation
        float3 kS = F;
        float3 kD = (1.0 - kS) * (1.0 - metalness);

        // final lighting
        float3 radiance = lightColor * attenuation * shadow * 10.0;
        float3 diffuse = (kD * color / PI);
        float3 lighting = (diffuse + specular) * radiance * NdotL;

        result += lighting;
#endif
    }
    
    return float4(result, 1.0);
}