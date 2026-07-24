//----------------------------------------
#define MAX_LIGHTS 6
#define MAX_SHADOWS_MAPS 4

#define LIGHT_POINT 0
#define LIGHT_SPOT 1
#define LIGHT_DIRECTIONAL 2

#define PI 3.14159265
#define MIN_ROUGHNESS 0.045
#define EPSILON 0.00001

//----------------------------------------
struct Light
{
    int Type;
    float _pad0, _pad1, _pad2;
    float3 Pos;
    float _pad3;
    float3 Dir;
    float _pad4;
    float3 Color;
    float _pad5;
    float Radius;
    float Cutoff;
    int ShadowMapIndex;
    float ShadowSoftness;
    matrix View;
    matrix Projection;
};

//----------------------------------------
static const int PCSS_BLOCKER_SAMPLES = 24;
static const int PCSS_FILTER_SAMPLES = 48;
static const float SHADOW_NEAR_PLANE = 0.1;
static const float GOLDEN_ANGLE = 2.39996323;

float LinearizeShadowDepth(float depth, float farPlane)
{
    return (SHADOW_NEAR_PLANE * farPlane) /
        max(farPlane - depth * (farPlane - SHADOW_NEAR_PLANE), EPSILON);
}

float ShadowInterleavedNoise(float2 pixelPosition)
{
    float2 pixel = floor(pixelPosition);
    return frac(52.9829189 * frac(dot(pixel, float2(0.06711056, 0.00583715))));
}

float2 VogelDiskSample(int sampleIndex, int sampleCount, float rotationAngle)
{
    float radius = sqrt((sampleIndex + 0.5) / sampleCount);
    float angle = sampleIndex * GOLDEN_ANGLE + rotationAngle;
    float sine = 0.0;
    float cosine = 1.0;
    sincos(angle, sine, cosine);
    return float2(cosine, sine) * radius;
}

float CalcSpotShadows(
    float3 worldPos,
    float3 normal,
    float3 toLight,
    float2 pixelPosition,
    float lightRadius,
    float shadowSoftness,
    matrix lightView,
    matrix lightProjection,
    Texture2D shadowMap,
    SamplerState shadowDepthSampler,
    SamplerComparisonState shadowSampler
)
{
    float4 lightSpacePos = mul(float4(worldPos, 1.0), lightView);
    lightSpacePos = mul(lightSpacePos, lightProjection);
			
    lightSpacePos.xyz /= lightSpacePos.w;

    float2 shadowUV = float2(
		lightSpacePos.x * 0.5 + 0.5,
		-lightSpacePos.y * 0.5 + 0.5
	);
    
    float shadowDepth = lightSpacePos.z;
			
    if (shadowUV.x < 0.0 || shadowUV.x > 1.0 ||
        shadowUV.y < 0.0 || shadowUV.y > 1.0 ||
        shadowDepth < 0.0 || shadowDepth > 1.0)
        return 1.0;

        // Increase the bias on grazing surfaces, where shadow acne is most visible,
        // while keeping front-facing surfaces close to the caster.
        float NdotL = saturate(dot(normalize(normal), normalize(toLight)));
        float bias = lerp(0.00002, 0.00025, 1.0 - NdotL);
        uint shadowWidth = 1;
        uint shadowHeight = 1;
        shadowMap.GetDimensions(shadowWidth, shadowHeight);
				
        float2 texelSize = 1.0 / float2(shadowWidth, shadowHeight);
        float receiverDepth = LinearizeShadowDepth(shadowDepth, lightRadius);
        float searchRadius = max(
            shadowSoftness * saturate((receiverDepth - SHADOW_NEAR_PLANE) / receiverDepth),
            max(texelSize.x, texelSize.y)
        );
        float shadowNoise = ShadowInterleavedNoise(pixelPosition);
        float kernelRotation = shadowNoise * 2.0 * PI;
        // Step 1: find occluders around the receiver.
        float blockerDepthSum = 0.0;
        int blockerCount = 0;

        [unroll]
        for (int blockerIndex = 0; blockerIndex < PCSS_BLOCKER_SAMPLES; ++blockerIndex)
        {
            float2 diskOffset = VogelDiskSample(
                blockerIndex,
                PCSS_BLOCKER_SAMPLES,
                kernelRotation
            );
            float2 sampleUV = saturate(shadowUV + diskOffset * searchRadius);
            float sampledDepth = shadowMap.SampleLevel(shadowDepthSampler, sampleUV, 0).r;

            if (sampledDepth < shadowDepth - bias)
            {
                blockerDepthSum += LinearizeShadowDepth(sampledDepth, lightRadius);
                blockerCount++;
            }
        }

        if (blockerCount == 0)
            return 1.0;

        // Step 2: estimate penumbra size from receiver/blocker separation.
        float averageBlockerDepth = blockerDepthSum / blockerCount;
        float penumbraRatio = max(receiverDepth - averageBlockerDepth, 0.0) /
            max(averageBlockerDepth, EPSILON);
        float filterRadius = clamp(
            shadowSoftness * penumbraRatio,
            max(texelSize.x, texelSize.y),
            shadowSoftness * 4.0
        );

        // Step 3: variable-radius PCF produces contact-hardening soft shadows.
        float shadowSum = 0.0;

        [unroll]
        for (int filterIndex = 0; filterIndex < PCSS_FILTER_SAMPLES; ++filterIndex)
        {
            float2 diskOffset = VogelDiskSample(
                filterIndex,
                PCSS_FILTER_SAMPLES,
                kernelRotation
            );
            float2 sampleUV = saturate(shadowUV + diskOffset * filterRadius);
            shadowSum += shadowMap.SampleCmpLevelZero(
                shadowSampler,
                sampleUV,
                shadowDepth - bias
            );
        }

    float filteredShadow = shadowSum / PCSS_FILTER_SAMPLES;
    float quantizationDither = (shadowNoise - 0.5) / PCSS_FILTER_SAMPLES;
    return saturate(filteredShadow + quantizationDither);
}

float CalcDirectionalShadows(
    float3 worldPos,
    float3 normal,
    float3 toLight,
    matrix lightView,
    matrix lightProjection,
    Texture2D shadowMap,
    SamplerComparisonState shadowSampler
)
{
    float4 lightSpacePos = mul(float4(worldPos, 1.0), lightView);
    lightSpacePos = mul(lightSpacePos, lightProjection);
    lightSpacePos.xyz /= max(lightSpacePos.w, EPSILON);

    float2 shadowUV = float2(
        lightSpacePos.x * 0.5 + 0.5,
        -lightSpacePos.y * 0.5 + 0.5
    );
    float shadowDepth = lightSpacePos.z;

    if (shadowUV.x < 0.0 || shadowUV.x > 1.0 ||
        shadowUV.y < 0.0 || shadowUV.y > 1.0 ||
        shadowDepth < 0.0 || shadowDepth > 1.0)
        return 1.0;

    float NdotL = saturate(dot(normalize(normal), normalize(toLight)));
    float bias = lerp(0.00005, 0.0004, 1.0 - NdotL);

    uint shadowWidth = 1;
    uint shadowHeight = 1;
    shadowMap.GetDimensions(shadowWidth, shadowHeight);
    float2 texelSize = 1.0 / float2(shadowWidth, shadowHeight);

    float shadowSum = 0.0;

    [unroll]
    for (int x = -1; x <= 1; ++x)
    {
        [unroll]
        for (int y = -1; y <= 1; ++y)
        {
            shadowSum += shadowMap.SampleCmpLevelZero(
                shadowSampler,
                shadowUV + float2(x, y) * texelSize,
                shadowDepth - bias
            );
        }
    }

    return shadowSum / 9.0;
}

//----------------------------------------
float CalcSimpleAmbient()
{
    return 0.08;
}

float3 CalcSimpleLightDiffuse(
    float3 normal,
    float3 toLight,
    float3 color
)
{
    float diff = max(dot(normal, toLight), 0.0);
    return diff * color;
}

// blinn phong
float3 CalcSimpleLightSpecular(
    float3 normal,
    float3 toLight, 
    float3 toView
)
{
    float3 H = normalize(toLight + toView);
    float specAngle = max(dot(normal, H), 0.0);
        
    float ks = 1.5;
    float shininess = 32.0;
    return ks * pow(specAngle, shininess);
}

//----------------------------------------
// PBR
float3 FresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float3 FresnelSchlickRoughness(float cosTheta, float3 F0, float roughness)
{
    return F0 +
        (max((1.0 - roughness).xxx, F0) - F0) *
        pow(1.0 - cosTheta, 5.0);
}

float2 EnvBRDFApprox(float roughness, float NdotV)
{
    const float4 c0 = float4(-1.0, -0.0275, -0.572, 0.022);
    const float4 c1 = float4(1.0, 0.0425, 1.04, -0.04);
    float4 r = roughness * c0 + c1;
    float a004 = min(r.x * r.x, exp2(-9.28 * NdotV)) * r.x + r.y;
    return float2(-1.04, 1.04) * a004 + r.zw;
}

float DistributionGGX(float3 N, float3 H, float roughness)
{
    roughness = clamp(roughness, MIN_ROUGHNESS, 1.0);
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = saturate(dot(N, H));
    float NdotH2 = NdotH * NdotH;

    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    return a2 / max(PI * denom * denom, EPSILON);
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    roughness = clamp(roughness, MIN_ROUGHNESS, 1.0);
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;
    return NdotV / max(NdotV * (1.0 - k) + k, EPSILON);
}

float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}

// Inverse-square falloff with a smooth, finite radius. Clamping the minimum
// distance avoids singular highlights when a light intersects geometry.
float CalcDistanceAttenuation(float distanceToLight, float radius)
{
    float safeRadius = max(radius, 0.001);
    float normalizedDistance = distanceToLight / safeRadius;
    float window = saturate(1.0 - pow(normalizedDistance, 4.0));
    window *= window;

    return window / max(distanceToLight * distanceToLight, 1.0);
}

//----------------------------------------
// IBL
float3 CalcIBL(
    float3 normal,
    float3 toView,
    float3 color,
    float metalness,
    float roughness,
    float ao,
    TextureCube cubemap,
    SamplerState samp
)
{
    float3 N = normalize(normal);
    float3 V = normalize(toView);
    float NdotV = saturate(dot(N, V));
    roughness = clamp(roughness, MIN_ROUGHNESS, 1.0);

    float3 F0 = lerp(float3(0.04, 0.04, 0.04), color, metalness);
    float3 R = reflect(-V, N);

    uint environmentWidth = 1;
    uint environmentHeight = 1;
    uint environmentMipLevels = 1;
    cubemap.GetDimensions(0, environmentWidth, environmentHeight, environmentMipLevels);
    float maxMipLevel = max((float)environmentMipLevels - 1.0, 0.0);

    // The last mip provides a cheap diffuse irradiance approximation. The
    // reflection mip selected by roughness acts as a prefiltered environment.
    float3 irradiance = cubemap.SampleLevel(samp, N, maxMipLevel).rgb;
    float3 prefilteredColor = cubemap.SampleLevel(samp, R, roughness * maxMipLevel).rgb;

    float3 F = FresnelSchlickRoughness(NdotV, F0, roughness);
    float3 kS = F;
    float3 kD = (1.0 - kS) * (1.0 - metalness);
    float3 diffuseIBL = irradiance * color;

    float2 envBRDF = EnvBRDFApprox(roughness, NdotV);
    float3 specularIBL = prefilteredColor * (F0 * envBRDF.x + envBRDF.y);

    return (kD * diffuseIBL + specularIBL) * ao;
}
