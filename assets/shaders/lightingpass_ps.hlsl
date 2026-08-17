//----------------------------------------
#include "lighting.hlsl"

#define SIMPLE_LIGHTING 0
#define NO_SHADOWS 1

//----------------------------------------
Texture2D g_tColor : register(t0);
Texture2D g_tNormal : register(t1);
Texture2D g_tPosition : register(t2);
Texture2D g_tSpecular : register(t3);

#if !NO_SHADOWS
Texture2D ShadowMaps0 : register(t4);
Texture2D ShadowMaps1 : register(t5);
Texture2D ShadowMaps2 : register(t6);
Texture2D ShadowMaps3 : register(t7);
#endif

TextureCube g_tCubemap : register(t10);

//----------------------------------------
SamplerState g_sSampler : register(s0);
#if !NO_SHADOWS
SamplerComparisonState ShadowSampler : register(s1);
SamplerState ShadowDepthSampler : register(s2);
#endif

//----------------------------------------
cbuffer LightsData : register(b0)
{
    Light Lights[MAX_LIGHTS];
    uint LightCount;
    float3 _pad;
};

cbuffer ViewData : register(b1)
{
    matrix View;
    matrix InvView;
};

//----------------------------------------
struct PS_INPUT
{
    float4 vPos : SV_POSITION;
    float2 vUV : TEXCOORD0;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    int2 pixel = int2(input.vPos.xy);

    // gbuffer
    float4 pos = g_tPosition.Load(int3(pixel, 0));
    if (pos.a < 0.5)
        return float4(0.0, 0.0, 0.0, 1.0);

    float3 color = g_tColor.Load(int3(pixel, 0)).rgb;
    float3 viewPos = pos.rgb;
    float4 norm = g_tNormal.Load(int3(pixel, 0));
    float3 normal = normalize(norm.rgb * 2.0 - 1.0);
    float3 spec = g_tSpecular.Load(int3(pixel, 0)).rgb;
    
    float emission = norm.a;
    float roughness = clamp(spec.r, MIN_ROUGHNESS, 1.0);
    float metalness = saturate(spec.g);
    float ao = saturate(spec.b);

    float3 worldPos = mul(float4(viewPos, 1.0), InvView).xyz;

    float3 toView = normalize(-viewPos); // view direction (to camera)
    float3 toViewWorld = normalize(mul(float4(toView, 0.0), InvView).xyz);

    float3 normalWorld = normalize(
        mul(float4(normal, 0.0), InvView).xyz
    );

#if SIMPLE_LIGHTING
    float3 result = CalcSimpleAmbient() * color * ao;
#else
    float3 result = CalcIBL(normalWorld, toViewWorld, color, metalness, roughness, ao, g_tCubemap, g_sSampler);
#endif

    float3 ambientLighting = result;
    float3 directLighting = 0.0;
    
    // lighting
    [loop]
    for (uint i = 0; i < LightCount; i++)
    {
        int lightType = Lights[i].Type;
        float lightRadius = Lights[i].Radius;
        float lightOuterCutoff = Lights[i].OuterCutoff;
        float lightInnerCutoff = Lights[i].InnerCutoff;
        float shadowSoftness = Lights[i].ShadowSoftness;
        float3 lightPos = Lights[i].Pos;
        float3 lightColor = Lights[i].Color;
        float3 lightBrightness = Lights[i].Brightness;
        float3 lightDir = Lights[i].Dir;
        matrix lightView = Lights[i].View;
        matrix lightProjection = Lights[i].Projection;
        
        //----------------------------------------
        float3 lightPosVS = mul(float4(lightPos, 1.0), View).xyz;
        float3 lightDirVS = normalize(mul(float4(lightDir, 0.0), View).xyz);

        float3 lightVector = lightPosVS - viewPos;
        float distance = length(lightVector);

        float3 toLight = lightVector / max(distance, EPSILON);
        float3 toLightWorld = normalize(mul(float4(toLight, 0.0), InvView).xyz);

        float attenuation = 1.0;
		float shadow = 1.0;
        
        //----------------------------------------
        if (lightType == LIGHT_POINT)
        {
            attenuation = CalcDistanceAttenuation(distance, lightRadius);
        }
        else if (lightType == LIGHT_SPOT)
        {
            float att = CalcDistanceAttenuation(distance, lightRadius);

            float theta = dot(toLight, -lightDirVS);

            float spotEffect = smoothstep(lightOuterCutoff, lightInnerCutoff, theta);
            attenuation = att * spotEffect;

#if !NO_SHADOWS
            int index = Lights[i].ShadowMapIndex;

            if (index >= 0)
            {
                if (index == 0)
                    shadow = CalcSpotShadows(worldPos, normalWorld, toLightWorld, input.vPos.xy, lightRadius, shadowSoftness, lightView, lightProjection, ShadowMaps0, ShadowDepthSampler, ShadowSampler);
                else if (index == 1)
                    shadow = CalcSpotShadows(worldPos, normalWorld, toLightWorld, input.vPos.xy, lightRadius, shadowSoftness, lightView, lightProjection, ShadowMaps1, ShadowDepthSampler, ShadowSampler);
                else if (index == 2)
                    shadow = CalcSpotShadows(worldPos, normalWorld, toLightWorld, input.vPos.xy, lightRadius, shadowSoftness, lightView, lightProjection, ShadowMaps2, ShadowDepthSampler, ShadowSampler);
                else if (index == 3)
                    shadow = CalcSpotShadows(worldPos, normalWorld, toLightWorld, input.vPos.xy, lightRadius, shadowSoftness, lightView, lightProjection, ShadowMaps3, ShadowDepthSampler, ShadowSampler);
            }
#endif
        }
        else if (lightType == LIGHT_DIRECTIONAL)
        {
            toLight = -lightDirVS;
            toLightWorld = -normalize(lightDir);

            attenuation = 1.0;

#if !NO_SHADOWS
            int index = Lights[i].ShadowMapIndex;

            if (index >= 0)
            {
                if (index == 0)
                    shadow = CalcDirectionalShadows(worldPos, normalWorld, toLightWorld, lightView, lightProjection, ShadowMaps0, ShadowSampler);
                else if (index == 1)
                    shadow = CalcDirectionalShadows(worldPos, normalWorld, toLightWorld, lightView, lightProjection, ShadowMaps1, ShadowSampler);
                else if (index == 2)
                    shadow = CalcDirectionalShadows(worldPos, normalWorld, toLightWorld, lightView, lightProjection, ShadowMaps2, ShadowSampler);
                else if (index == 3)
                    shadow = CalcDirectionalShadows(worldPos, normalWorld, toLightWorld, lightView, lightProjection, ShadowMaps3, ShadowSampler);
            }
#endif
        }

        float3 lightContribution = 0.0;
        
#if SIMPLE_LIGHTING
        //----------------------------------------
        // Diffuse
        float3 diffuse = CalcSimpleLightDiffuse(normalWorld, toLightWorld, color) * (lightColor * lightBrightness) * attenuation * shadow;

        //----------------------------------------
        // specular (blinn phong)
        float3 specularColor = CalcSimpleLightSpecular(normalWorld, toLightWorld, toViewWorld) * (lightColor * lightBrightness) * attenuation * shadow;
        
        lightContribution = diffuse + specularColor;
#else
        //-------------------- PBR Cook-Torrance --------------------
        float3 N = normal;
        float3 V = normalize(toView);
        float3 L = normalize(toLight);
        float3 H = normalize(V + L);

        float NdotL = saturate(dot(N, L));
        float NdotV = saturate(dot(N, V));
        float HdotV = saturate(dot(H, V));

        // base reflectivity (dielectric = 0.04, metal = albedo)
        float3 F0 = lerp(float3(0.04, 0.04, 0.04), color, metalness);

        // microfacet terms
        float3 F = FresnelSchlick(HdotV, F0);
        float D = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);

        // cook torrance BRDF
        float3 numerator = D * G * F;
        float denominator = max(4.0 * NdotV * NdotL, EPSILON);
        float3 specular = numerator / denominator;

        // energy conservation
        float3 kS = F;
        float3 kD = (1.0 - kS) * (1.0 - metalness);

        // final lighting
        float3 radiance = (lightColor * lightBrightness) * attenuation * shadow * 10.0;
        float3 diffuse = (kD * color / PI);
        float3 lighting = (diffuse + specular) * radiance * NdotL;

        lightContribution = lighting;
#endif

        result += lightContribution;
        directLighting += lightContribution;
    }
    
    result += color * emission;

    return float4(result, 1.0);
}
