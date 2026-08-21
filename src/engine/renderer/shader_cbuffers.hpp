#pragma once

#include "renderer/lighting.hpp"

namespace render
{
    struct CBTransform
    {
        glm::mat4 Model;
        glm::mat4 View;
        glm::mat4 Projection;
        glm::mat4 ModelView;
        glm::mat4 InvModelView;
    };

    struct CBViewData
    {
        glm::mat4 View;
        glm::mat4 InvView;
    };

    struct CBTint
    {
        glm::vec4 Tint;
        float _pad0, _pad1, _pad2;
        float Emission;
    };

    struct alignas(16) CBLight
    {
        int Type;
        float _pad0, _pad1, _pad2;
        glm::vec3 Pos;
        float _pad3;
        glm::vec3 Dir;
        float Brightness;
        glm::vec3 Color;
        float Radius;
        float OuterCutoff;
        float InnerCutoff;
        int ShadowMapIndex;
        float ShadowSoftness;
        glm::mat4 View;
        glm::mat4 Projection;
    };

    struct CBLightsData
    {
        CBLight Lights[ lighting::MAX_LIGHTS ];
        std::uint32_t LightCount;
        std::uint32_t _pad0;
        std::uint32_t _pad1;
        std::uint32_t _pad2;
    };

    enum MaterialTextureFlags : std::uint32_t
    {
        MATERIAL_HAS_COLOR_MAP = 1u << 0,
        MATERIAL_HAS_NORMAL_MAP = 1u << 1,
        MATERIAL_HAS_ROUGH_MAP = 1u << 2,
        MATERIAL_HAS_METAL_MAP = 1u << 3,
        MATERIAL_HAS_AO_MAP = 1u << 4,
        MATERIAL_HAS_HEIGHT_MAP = 1u << 5
    };

    struct alignas(16) CBMaterialFactors
    {
        glm::vec4 ColorFactor{1.0f};

        float NormalFactor = 1.0f;
        float RoughFactor = 1.0f;
        float MetalFactor = 1.0f;
        float AOFactor = 1.0f;
        float HeightFactor = 1.0f;

        std::uint32_t TextureMask = 0;
        float _pad0 = 0.0f;
        float _pad1 = 0.0f;
    };

    static_assert( sizeof( CBMaterialFactors ) == 48 );
}
