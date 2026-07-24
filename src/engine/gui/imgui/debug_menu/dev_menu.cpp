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
                    "Selected light contribution",
                    "Direct lighting",
                    "Ambient lighting",
                    "Depth (linear)",
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

                ImGui::SeparatorText("Shadow tuning");
                ImGui::SliderInt(
                    "Light index",
                    &g_RenderDebugSettings.SelectedLight,
                    0,
                    MAX_LIGHTS - 1
                );

                auto& lights = LightManager::GetInstance().GetLights();
                const auto selectedLight = lights[ g_RenderDebugSettings.SelectedLight ];

                if ( selectedLight &&
                     selectedLight->IsA(LIGHT_SPOT) &&
                     selectedLight->IsShadowsEnabled() )
                {
                    float softness = selectedLight->GetShadowSoftness();
                    if ( ImGui::SliderFloat(
                        "PCSS softness",
                        &softness,
                        0.0f,
                        0.1f,
                        "%.3f"
                    ) )
                    {
                        selectedLight->SetShadowSoftness(softness);
                    }
                } else
                {
                    ImGui::TextDisabled("Selected light does not use PCSS");
                }

                ImGui::SeparatorText("Output");
                ImGui::SliderFloat(
                    "Exposure",
                    &g_RenderDebugSettings.Exposure,
                    0.0f,
                    4.0f,
                    "%.2f"
                );
                ImGui::Checkbox("Enable IBL", &g_RenderDebugSettings.IBLEnabled);

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
