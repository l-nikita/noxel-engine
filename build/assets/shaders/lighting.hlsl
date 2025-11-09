//----------------------------------------
#define MAX_LIGHTS 6
#define MAX_SHADOWS_MAPS 4

#define LIGHT_POINT 0
#define LIGHT_SPOT 1
#define LIGHT_DIRECTIONAL 2

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
    SamplerComparisonState shadowSampler)
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
        float bias = lerp(0.00015, 0.0001, saturate(1 - dot(normal, toLight)));
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
float CalcAmbient()
{
    return 0.05;
}