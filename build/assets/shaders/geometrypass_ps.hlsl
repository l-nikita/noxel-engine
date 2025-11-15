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
cbuffer ViewData : register(b0)
{
    float4 ViewPos;
};

cbuffer Tint : register(b1)
{
    float4 Tint;
    float _pad0, _pad1, _pad2;
    float Emission;
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

PS_OUTPUT main(PS_INPUT input) : SV_TARGET
{
    PS_OUTPUT o;
    
    float3 N = normalize(input.vNormal);
    float3 T = normalize(input.vTangent - dot(input.vTangent, N) * N);
    float3 B = cross(N, T);
    float3x3 TBN = float3x3(T, B, N);
    float2 uv = input.vUV;

    float3 texColor = Color.Sample(Samp, uv);
    float4 color = input.vColor * Tint;
    
    if (texColor.r > 0 || texColor.g > 0 || texColor.b > 0)
        color = Color.Sample(Samp, uv) * input.vColor * Tint;
    
    float3 normal = Normal.Sample(Samp, uv).xyz * 2.0 - 1.0;
    float roughness = Roughness.Sample(Samp, uv).r;
    float metalness = Metalness.Sample(Samp, uv).r;
    float height = Height.Sample(Samp, uv).r;
    float ao = AO.Sample(Samp, uv).r;
    float3 finalNormal = normalize(mul(normal, TBN));

    o.gColor = color;
    o.gNormal = float4(finalNormal, Emission);
    o.gPosition = float4(input.vWorldPos, 1.0);
    o.gSpecular = float4(roughness, metalness, ao, height);

    return o;
}