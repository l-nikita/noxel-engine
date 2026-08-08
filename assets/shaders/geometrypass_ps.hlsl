//----------------------------------------
Texture2D Color         : register(t0);
Texture2D Normal        : register(t1);
Texture2D Roughness     : register(t2);
Texture2D Metalness     : register(t3);
Texture2D AO            : register(t4);
Texture2D Height        : register(t5);

//----------------------------------------
SamplerState Samp : register(s0);

//----------------------------------------
#define MATERIAL_HAS_COLOR_MAP  (1u << 0)
#define MATERIAL_HAS_NORMAL_MAP (1u << 1)
#define MATERIAL_HAS_ROUGH_MAP  (1u << 2)
#define MATERIAL_HAS_METAL_MAP  (1u << 3)
#define MATERIAL_HAS_AO_MAP     (1u << 4)
#define MATERIAL_HAS_HEIGHT_MAP (1u << 5)

cbuffer Tint : register(b0)
{
    float4 Tint;
    float _pad0, _pad1, _pad2;
    float Emission;
};

cbuffer MaterialFactors : register(b1)
{
    float4 ColorFactor;

    float NormalFactor;
    float RoughFactor;
    float MetalFactor;
    float AOFactor;

    float HeightFactor;
    uint TextureMask;
    float _matPad0;
    float _matPad1;
};

//----------------------------------------
struct PS_INPUT
{
    float4 vPos         : SV_POSITION;
    float3 vWorldPos    : POSITION;
    float3 vNormal      : NORMAL;
    float3 vTangent     : TANGENT;
    float3 vBitangent   : BINORMAL;
    float2 vUV          : TEXCOORD0;
    float4 vColor       : COLOR;
};
	
struct PS_OUTPUT
{
    float4 gColor       : SV_Target0;
    float4 gNormal      : SV_Target1;
    float4 gPosition    : SV_Target2;
    float4 gSpecular    : SV_Target3;
};

PS_OUTPUT main(PS_INPUT input)
{
    PS_OUTPUT o;
    
    float3 N = normalize(input.vNormal);
    float3 T = normalize(input.vTangent - dot(input.vTangent, N) * N);
    float3 B = cross(N, T);
    float3x3 TBN = float3x3(T, B, N);
    float2 uv = input.vUV;

    float4 color = input.vColor * Tint * ColorFactor;
    if ((TextureMask & MATERIAL_HAS_COLOR_MAP) != 0)
        color *= Color.Sample(Samp, uv);

    float3 finalNormal = N;
    if ((TextureMask & MATERIAL_HAS_NORMAL_MAP) != 0)
    {
        float3 normal = Normal.Sample(Samp, uv).xyz * 2.0 - 1.0;
        normal.xy *= max(NormalFactor, 0.0);
        finalNormal = normalize(mul(normalize(normal), TBN));
    }

    float roughness = RoughFactor;
    if ((TextureMask & MATERIAL_HAS_ROUGH_MAP) != 0)
        roughness *= Roughness.Sample(Samp, uv).r;

    float metalness = MetalFactor;
    if ((TextureMask & MATERIAL_HAS_METAL_MAP) != 0)
        metalness *= Metalness.Sample(Samp, uv).r;

    float ao = AOFactor;
    if ((TextureMask & MATERIAL_HAS_AO_MAP) != 0)
        ao *= AO.Sample(Samp, uv).r;

    float height = HeightFactor;
    if ((TextureMask & MATERIAL_HAS_HEIGHT_MAP) != 0)
        height *= Height.Sample(Samp, uv).r;

    roughness = saturate(roughness);
    metalness = saturate(metalness);
    ao = saturate(ao);
    height = saturate(height);

    o.gColor = color;
    o.gNormal = float4(finalNormal, Emission);
    o.gPosition = float4(input.vWorldPos, 1.0);
    o.gSpecular = float4(roughness, metalness, ao, height);

    return o;
}
