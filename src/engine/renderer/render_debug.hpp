#pragma once

enum LightingDebugView
{
    LIGHTING_DEBUG_LIT = 0,
    LIGHTING_DEBUG_ALBEDO,
    LIGHTING_DEBUG_NORMALS,
    LIGHTING_DEBUG_WORLD_POSITION,
    LIGHTING_DEBUG_ROUGHNESS,
    LIGHTING_DEBUG_METALNESS,
    LIGHTING_DEBUG_AO,
    LIGHTING_DEBUG_EMISSION,
    LIGHTING_DEBUG_SHADOW_FACTOR,
    LIGHTING_DEBUG_SELECTED_LIGHT,
    LIGHTING_DEBUG_DIRECT_LIGHTING,
    LIGHTING_DEBUG_AMBIENT_LIGHTING,
    LIGHTING_DEBUG_DEPTH,
};

struct RenderDebugSettings
{
    int LightingView = LIGHTING_DEBUG_LIT;
    int SelectedLight = 1;
    float OverlayOpacity = 1.0f;
    float Exposure = 1.35f;
    bool IBLEnabled = true;
};

inline RenderDebugSettings g_RenderDebugSettings;
