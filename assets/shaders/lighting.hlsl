//----------------------------------------
#define MAX_LIGHTS 6
#define MAX_SHADOWS_MAPS 4

#define LIGHT_POINT 0
#define LIGHT_SPOT 1
#define LIGHT_DIRECTIONAL 2

#define PI 3.14159265

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
    float _pad6;
    matrix View;
    matrix Projection;
};

//----------------------------------------
float CalcSpotShadows(
    float3 worldPos,
    float3 normal,
    float3 toLight,
    matrix lightView,
    matrix lightProjection,
    Texture2D shadowMap, 
    SamplerComparisonState shadowSampler
)
{
    float shadow = 1.0;
		
    float4 lightSpacePos = mul(float4(worldPos, 1.0), lightView);
    lightSpacePos = mul(lightSpacePos, lightProjection);
			
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
        float bias = lerp(0.0001, 0.00015, saturate(1 - dot(normal, toLight)));
        uint shadowWidth, shadowHeight;
        shadowMap.GetDimensions(shadowWidth, shadowHeight);
				
        float2 texelSize = 1.0 / float2(shadowWidth, shadowHeight);
        float shadowSum = 0.0;
				
        const int range = 1; // 1 = 3x3, 2 = 5x5

        for (int x = -range; x <= range; ++x)
        {
            for (int y = -range; y <= range; ++y)
            {
                float2 offset = float2(x, y) * texelSize;
                shadowSum += shadowMap.SampleCmpLevelZero(shadowSampler, shadowUV + offset, shadowDepth - bias);
            }
        }
				
        shadow = shadowSum / pow((range * 2 + 1), 2);
    }
    
    return shadow;
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
        
    float ks = 1.5; // коэффициент блеска
    float shininess = 32.0; // ширина блика
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
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    return a2 / (PI * denom * denom);
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;
    return NdotV / (NdotV * (1.0 - k) + k);
}

float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
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