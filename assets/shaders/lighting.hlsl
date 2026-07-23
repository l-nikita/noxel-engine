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
static const int PCSS_BLOCKER_SAMPLES = 16;
static const int PCSS_FILTER_SAMPLES = 16;
static const float SHADOW_NEAR_PLANE = 0.1;

static const float2 PCSS_DISK[16] =
{
    float2(-0.94201624, -0.39906216),
    float2( 0.94558609, -0.76890725),
    float2(-0.09418410, -0.92938870),
    float2( 0.34495938,  0.29387760),
    float2(-0.91588581,  0.45771432),
    float2(-0.81544232, -0.87912464),
    float2(-0.38277543,  0.27676845),
    float2( 0.97484398,  0.75648379),
    float2( 0.44323325, -0.97511554),
    float2( 0.53742981, -0.47373420),
    float2(-0.26496911, -0.41893023),
    float2( 0.79197514,  0.19090188),
    float2(-0.24188840,  0.99706507),
    float2(-0.81409955,  0.91437590),
    float2( 0.19984126,  0.78641367),
    float2( 0.14383161, -0.14100790)
};

float LinearizeShadowDepth(float depth, float farPlane)
{
    return (SHADOW_NEAR_PLANE * farPlane) /
        max(farPlane - depth * (farPlane - SHADOW_NEAR_PLANE), EPSILON);
}

float2 RotateDiskSample(float2 sampleOffset, float2x2 rotation)
{
    return mul(sampleOffset, rotation);
}

float2x2 CalcShadowKernelRotation(float2 pixelPosition)
{
    float noise = frac(52.9829189 * frac(dot(pixelPosition, float2(0.06711056, 0.00583715))));
    float angle = noise * 2.0 * PI;
    float sine = 0.0;
    float cosine = 1.0;
    sincos(angle, sine, cosine);

    return float2x2(cosine, -sine, sine, cosine);
}

float CalcSpotShadows(
    float3 worldPos,
    float3 normal,
    float3 toLight,
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
        float bias = lerp(0.0001, 0.001, 1.0 - NdotL);
        uint shadowWidth = 1;
        uint shadowHeight = 1;
        shadowMap.GetDimensions(shadowWidth, shadowHeight);
				
        float2 texelSize = 1.0 / float2(shadowWidth, shadowHeight);
        float receiverDepth = LinearizeShadowDepth(shadowDepth, lightRadius);
        float searchRadius = max(
            shadowSoftness * saturate((receiverDepth - SHADOW_NEAR_PLANE) / receiverDepth),
            max(texelSize.x, texelSize.y)
        );
        float2x2 kernelRotation = CalcShadowKernelRotation(shadowUV * float2(shadowWidth, shadowHeight));

        // Step 1: find occluders around the receiver.
        float blockerDepthSum = 0.0;
        int blockerCount = 0;

        [unroll]
        for (int blockerIndex = 0; blockerIndex < PCSS_BLOCKER_SAMPLES; ++blockerIndex)
        {
            float2 diskOffset = RotateDiskSample(PCSS_DISK[blockerIndex], kernelRotation);
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
            float2 diskOffset = RotateDiskSample(PCSS_DISK[filterIndex], kernelRotation);
            float2 sampleUV = saturate(shadowUV + diskOffset * filterRadius);
            shadowSum += shadowMap.SampleCmpLevelZero(
                shadowSampler,
                sampleUV,
                shadowDepth - bias
            );
        }

    return shadowSum / PCSS_FILTER_SAMPLES;
}

//----------------------------------------
float CalcSimpleAmbient()
{
    return 0.05;
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
    float NdotV = max(dot(N, V), 0.0);

    float3 F0 = lerp(float3(0.04, 0.04, 0.04), color, metalness);
    float3 R = reflect(-V, N);

    float3 envColor = cubemap.Sample(samp, R).rgb;
    envColor = lerp(envColor, dot(envColor, float3(0.333, 0.333, 0.333)).xxx, roughness);
    
    float3 F = FresnelSchlick(NdotV, F0);
    float3 specularIBL = envColor * F;
    float3 ambientIBL = color * 0.2 * (1.0 - metalness);

    return (ambientIBL + specularIBL) * ao;
}
