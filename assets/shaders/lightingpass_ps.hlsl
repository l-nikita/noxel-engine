//----------------------------------------
#include "lighting.hlsl"

#define SIMPLE_LIGHTING 0

#define DEBUG_LIT 0
#define DEBUG_ALBEDO 1
#define DEBUG_NORMALS 2
#define DEBUG_WORLD_POSITION 3
#define DEBUG_ROUGHNESS 4
#define DEBUG_METALNESS 5
#define DEBUG_AO 6
#define DEBUG_EMISSION 7
#define DEBUG_SHADOW_FACTOR 8
#define DEBUG_ATTENUATION 9
#define DEBUG_SELECTED_LIGHT 10
#define DEBUG_DIRECT_LIGHTING 11
#define DEBUG_AMBIENT_LIGHTING 12
#define DEBUG_CAMERA_DISTANCE 13
#define DEBUG_WORLD_GRID 14

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
SamplerState ShadowDepthSampler : register(s2);

//----------------------------------------
cbuffer LightsData : register(b0)
{
    Light Lights[MAX_LIGHTS];
    float3 ViewPos;
    int DebugView;
    int DebugLightIndex;
    float DebugOverlayOpacity;
    float DebugVisualizationScale;
    float DebugExposure;
};

//----------------------------------------
struct PS_INPUT
{
    float4 vPos : SV_POSITION;
    float2 vUV : TEXCOORD0;
};

float3 DebugHeatmap(float value)
{
    value = saturate(value);
    float3 blue = float3(0.0, 0.1, 1.0);
    float3 cyan = float3(0.0, 1.0, 1.0);
    float3 yellow = float3(1.0, 1.0, 0.0);
    float3 red = float3(1.0, 0.0, 0.0);

    float3 cold = lerp(blue, cyan, saturate(value * 3.0));
    float3 warm = lerp(yellow, red, saturate((value - 0.66) * 3.0));
    return lerp(cold, warm, saturate((value - 0.33) * 1.5));
}

float4 main(PS_INPUT input) : SV_TARGET
{
    // gbuffer
    float3 color = gColor.Sample(Samp, input.vUV).rgb;
    float4 norm = gNormal.Sample(Samp, input.vUV);
    float3 normal = normalize(norm.rgb);
    float3 worldPos = gPosition.Sample(Samp, input.vUV).rgb;
    float3 spec = gSpecular.Sample(Samp, input.vUV).rgb;
    
    float emission = norm.a;
    float roughness = clamp(spec.r, MIN_ROUGHNESS, 1.0);
    float metalness = saturate(spec.g);
    float ao = saturate(spec.b);

    float3 toView = normalize(ViewPos - worldPos); // view direction (to camera)

#if SIMPLE_LIGHTING
    float3 result = CalcSimpleAmbient() * color * ao;
#else
    float3 result = CalcSimpleAmbient() * color * (1.0 - metalness) * ao;
#endif

    float3 ambientLighting = result;
    float3 directLighting = 0.0;
    float selectedShadow = 1.0;
    float selectedAttenuation = 0.0;
    float3 selectedLightContribution = 0.0;
    
    // lighting
    [unroll]
    for (int i = 0; i < MAX_LIGHTS; i++)
    {
        int lightType = Lights[i].Type;
        float lightRadius = Lights[i].Radius;
        float lightCutoff = Lights[i].Cutoff;
        float shadowSoftness = Lights[i].ShadowSoftness;
        float3 lightPos = Lights[i].Pos;
        float3 lightColor = Lights[i].Color;
        float3 lightDir = Lights[i].Dir;
        matrix lightView = Lights[i].View;
        matrix lightProjection = Lights[i].Projection;
        
        //----------------------------------------
        float3 lightVector = lightPos - worldPos;
        float distance = length(lightVector);
        float3 toLight = lightVector / max(distance, EPSILON);
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

            float theta = dot(toLight, normalize(-lightDir));
            float outerCutoff = lightCutoff;
            float innerCutoff = lerp(outerCutoff, 1.0, 0.2);
			
            float spotEffect = smoothstep(outerCutoff, innerCutoff, theta);
            attenuation = att * spotEffect;
            
            int index = Lights[i].ShadowMapIndex;

            if (index >= 0)
            {
                if (index == 0)
                    shadow = CalcSpotShadows(worldPos, normal, toLight, lightRadius, shadowSoftness, lightView, lightProjection, ShadowMaps0, ShadowDepthSampler, ShadowSampler);
                else if (index == 1)
                    shadow = CalcSpotShadows(worldPos, normal, toLight, lightRadius, shadowSoftness, lightView, lightProjection, ShadowMaps1, ShadowDepthSampler, ShadowSampler);
                else if (index == 2)
                    shadow = CalcSpotShadows(worldPos, normal, toLight, lightRadius, shadowSoftness, lightView, lightProjection, ShadowMaps2, ShadowDepthSampler, ShadowSampler);
                else if (index == 3)
                    shadow = CalcSpotShadows(worldPos, normal, toLight, lightRadius, shadowSoftness, lightView, lightProjection, ShadowMaps3, ShadowDepthSampler, ShadowSampler);
            }

        }
        else if (lightType == LIGHT_DIRECTIONAL)
        {
            toLight = normalize(-lightDir);
            attenuation = 1.0;
        }

        float3 lightContribution = 0.0;
        
#if SIMPLE_LIGHTING
        //----------------------------------------
        // Diffuse
        float3 diffuse = CalcSimpleLightDiffuse(normal, toLight, color) * lightColor * attenuation * shadow;

        //----------------------------------------
        // specular (blinn phong)
        float3 specularColor = CalcSimpleLightSpecular(normal, toLight, toView) * lightColor * attenuation * shadow;
        
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
        float3 radiance = lightColor * attenuation * shadow * 10.0;
        float3 diffuse = (kD * color / PI);
        float3 lighting = (diffuse + specular) * radiance * NdotL;

        lightContribution = lighting;
#endif

        result += lightContribution;
        directLighting += lightContribution;

        if (i == DebugLightIndex)
        {
            selectedShadow = shadow;
            selectedAttenuation = attenuation;
            selectedLightContribution = lightContribution;
        }
    }
    
    result += color * emission;
    result *= max(DebugExposure, 0.0);

    float3 debugColor = result;

    if (DebugView == DEBUG_ALBEDO)
        debugColor = color;
    else if (DebugView == DEBUG_NORMALS)
        debugColor = normal * 0.5 + 0.5;
    else if (DebugView == DEBUG_WORLD_POSITION)
        debugColor = sin(worldPos * max(DebugVisualizationScale, 0.001)) * 0.5 + 0.5;
    else if (DebugView == DEBUG_ROUGHNESS)
        debugColor = roughness.xxx;
    else if (DebugView == DEBUG_METALNESS)
        debugColor = metalness.xxx;
    else if (DebugView == DEBUG_AO)
        debugColor = ao.xxx;
    else if (DebugView == DEBUG_EMISSION)
        debugColor = DebugHeatmap(emission);
    else if (DebugView == DEBUG_SHADOW_FACTOR)
        debugColor = selectedShadow.xxx;
    else if (DebugView == DEBUG_ATTENUATION)
        debugColor = DebugHeatmap(selectedAttenuation);
    else if (DebugView == DEBUG_SELECTED_LIGHT)
        debugColor = selectedLightContribution;
    else if (DebugView == DEBUG_DIRECT_LIGHTING)
        debugColor = directLighting;
    else if (DebugView == DEBUG_AMBIENT_LIGHTING)
        debugColor = ambientLighting;
    else if (DebugView == DEBUG_CAMERA_DISTANCE)
        debugColor = DebugHeatmap(length(ViewPos - worldPos) * max(DebugVisualizationScale, 0.001));
    else if (DebugView == DEBUG_WORLD_GRID)
    {
        float3 gridPosition = abs(frac(worldPos * max(DebugVisualizationScale, 0.001)) - 0.5);
        float gridLine = smoothstep(0.46, 0.5, max(gridPosition.x, max(gridPosition.y, gridPosition.z)));
        debugColor = lerp(result, float3(0.1, 1.0, 0.2), gridLine);
    }

    float overlayOpacity = DebugView == DEBUG_LIT ? 0.0 : saturate(DebugOverlayOpacity);
    return float4(lerp(result, debugColor, overlayOpacity), 1.0);
}
