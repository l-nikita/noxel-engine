#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include "dev_menu.hpp"
#include "renderer/irenderer.hpp"
#include "renderer/lighting.hpp"
#include "renderer/render_debug.hpp"
#include "renderer/rendercamera.hpp"

DevPanel::DevPanel()
{
    m_vsyncEnabled = g_Renderer->IsVSyncEnabled();
}

void DevPanel::Draw()
{
    if ( ImGui::Begin("Developer menu") )
    {
        if ( ImGui::BeginTabBar("MyTabBar") )
        {
            if ( ImGui::BeginTabItem("General") )
            {
                ImGui::SeparatorText("Camera");

                auto& transform = g_Renderer->GetCamera()->m_Transform;
                ImGui::InputFloat3("Position", glm::value_ptr(transform.Position));

                const auto rot = glm::degrees(glm::eulerAngles(transform.Rotation));
                glm::vec3 ang(rot.x, rot.y, rot.z);
                ImGui::InputFloat3("Rotation", glm::value_ptr(ang));

                if ( ImGui::Button("Reset") )
                {
                    transform.Position = glm::zero<glm::vec3>();
                    transform.Rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
                }

                ImGui::SeparatorText("Render");

                if ( ImGui::Checkbox("VSync", &m_vsyncEnabled) )
                {
                    g_Renderer->SetVSync(m_vsyncEnabled);
                }

                //if (ImGui.Checkbox("Wireframe", ref _wireframeEnabled))
                //{
                //    (Client.Render as OpenGLRender).WireframeEnabled = _wireframeEnabled;
                //}

                ImGui::EndTabItem();
            }

            if ( ImGui::BeginTabItem("Misc") )
            {
                ImGui::Checkbox("Demo window", &m_demoViewEnabled);
                ImGui::EndTabItem();
            }

            if ( ImGui::BeginTabItem("Visual debug") )
            {
                static const char* lightingViews[] = {
                    "Lit",
                    "Albedo",
                    "Normals",
                    "World position",
                    "Roughness",
                    "Metalness",
                    "Ambient occlusion",
                    "Emission",
                    "Shadow factor",
                    "Light attenuation",
                    "Selected light contribution",
                    "Direct lighting",
                    "Ambient lighting",
                    "Camera distance",
                    "World grid",
                };

                ImGui::SeparatorText("Lighting overlay");
                ImGui::Combo(
                    "View",
                    &g_RenderDebugSettings.LightingView,
                    lightingViews,
                    IM_ARRAYSIZE(lightingViews)
                );

                if ( g_RenderDebugSettings.LightingView != LIGHTING_DEBUG_LIT )
                {
                    ImGui::SliderFloat(
                        "Overlay opacity",
                        &g_RenderDebugSettings.OverlayOpacity,
                        0.0f,
                        1.0f,
                        "%.2f"
                    );
                }

                const bool lightView =
                    g_RenderDebugSettings.LightingView == LIGHTING_DEBUG_SHADOW_FACTOR ||
                    g_RenderDebugSettings.LightingView == LIGHTING_DEBUG_ATTENUATION ||
                    g_RenderDebugSettings.LightingView == LIGHTING_DEBUG_SELECTED_LIGHT;

                if ( lightView )
                {
                    ImGui::SliderInt(
                        "Light index",
                        &g_RenderDebugSettings.SelectedLight,
                        0,
                        MAX_LIGHTS - 1
                    );
                }

                const bool scaledView =
                    g_RenderDebugSettings.LightingView == LIGHTING_DEBUG_WORLD_POSITION ||
                    g_RenderDebugSettings.LightingView == LIGHTING_DEBUG_CAMERA_DISTANCE ||
                    g_RenderDebugSettings.LightingView == LIGHTING_DEBUG_WORLD_GRID;

                if ( scaledView )
                {
                    ImGui::SliderFloat(
                        "Visualization scale",
                        &g_RenderDebugSettings.VisualizationScale,
                        0.001f,
                        1.0f,
                        "%.3f",
                        ImGuiSliderFlags_Logarithmic
                    );
                }

                ImGui::SeparatorText("Output");
                ImGui::SliderFloat(
                    "Exposure",
                    &g_RenderDebugSettings.Exposure,
                    0.0f,
                    4.0f,
                    "%.2f"
                );

                if ( ImGui::Button("Reset visual debug") )
                    g_RenderDebugSettings = {};

                ImGui::SameLine();

                if ( ImGui::Button("Show final image") )
                    g_RenderDebugSettings.LightingView = LIGHTING_DEBUG_LIT;

                ImGui::Spacing();
                ImGui::TextDisabled("Shadow: white = lit, black = occluded");
                ImGui::TextDisabled("Heatmaps: blue = low, red = high");

                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        if ( m_demoViewEnabled )
            ImGui::ShowDemoWindow();
    }

    ImGui::End();
}
