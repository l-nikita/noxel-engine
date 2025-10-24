PixelShader
[
    Texture2D gColor : register(t0);
    Texture2D gNormal : register(t1);
    Texture2D gPosition : register(t2);
    SamplerState Samp : register(s0);
	
    cbuffer LightData : register(b0)
    {
        float3 LightPos;
        float pad0;
        float3 LightColor;
        float pad1;
        float3 CameraPos;
        float pad2;
    };

    struct PS_INPUT
    {
        float4 iPos : SV_POSITION;
        float2 iUV : TEXCOORD0;
    };
	
    float4 main(PS_INPUT input) : SV_TARGET
    {
        // --- Получаем данные из G-buffer ---
        float3 objectColor = gColor.Sample(Samp, input.iUV).rgb;

        float3 normal = normalize(gNormal.Sample(Samp, input.iUV).rgb * 2.0f - 1.0f); // из [0,1] -> [-1,1]
        float3 worldPos = gPosition.Sample(Samp, input.iUV).rgb;

        // --- Ambient ---
        float ambientStrength = 0.1;
        float3 ambient = ambientStrength * LightColor;

        // --- Diffuse ---
        float3 lightDir = normalize(LightPos - worldPos);
        float diff = max(dot(normal, lightDir), 0.0);
        float3 diffuse = diff * LightColor;

        // --- Specular (Blinn-Phong) ---
        float3 viewDir = normalize(CameraPos - worldPos);
        float3 halfDir = normalize(lightDir + viewDir); // Blinn-Phong
        float specularStrength = 0.5;
        float spec = pow(max(dot(normal, halfDir), 0.0), 32);
        float3 specular = specularStrength * spec * LightColor;

        // --- Итоговый цвет ---
        float3 result = objectColor * (ambient + diffuse) + specular;

        return float4(result, 1.0f);
    }
]

VertexShader
[
    struct VS_OUTPUT
    {
        float4 oPos : SV_POSITION;
        float2 oUV : TEXCOORD0;
    };

    VS_OUTPUT main(uint id : SV_VertexID)
    {
        VS_OUTPUT o;

        float2 pos[3] =
        {
            float2(-1.0f, -1.0f),
            float2(-1.0f, 3.0f),
            float2(3.0f, -1.0f)
        };

        o.oPos = float4(pos[id], 0.0f, 1.0f);
        o.oUV = float2((pos[id].x + 1.0f) * 0.5f, 1.0f - (pos[id].y + 1.0f) * 0.5f);

        return o;
    }
]