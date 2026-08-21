#pragma once

namespace render::lighting
{
    constexpr auto MAX_LIGHTS = 6;
    constexpr auto MAX_SHADOWS_MAPS = 4;
    constexpr auto SHADOW_MAP_SIZE = 4096;

    enum LightType
    {
        LIGHT_POINT,
        LIGHT_SPOT,
        LIGHT_DIRECTIONAL,
    };
}
