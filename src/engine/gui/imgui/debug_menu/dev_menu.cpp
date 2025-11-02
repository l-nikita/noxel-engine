#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include "iengine.hpp"
#include "dev_menu.hpp"
#include "renderer/irenderer.hpp"
#include "renderer/rendercamera.hpp"

DevPanel::DevPanel()
{
	m_vsyncEnabled = g_Renderer->IsVSyncEnabled();
}

void DevPanel::Draw()
{
	if (ImGui::Begin("Developer menu"))
	{
		if (ImGui::BeginTabBar("MyTabBar"))
		{
			if (ImGui::BeginTabItem("General"))
			{
                ImGui::SeparatorText("Camera");

                auto& transform = g_Renderer->GetCamera()->m_Transform;
                ImGui::InputFloat3("Position", glm::value_ptr(transform.Position) );

				auto rot = glm::degrees(glm::eulerAngles(transform.Rotation));
                glm::vec3 ang(rot.x, rot.y, rot.z);
                ImGui::InputFloat3("Rotation", glm::value_ptr(ang));

                if (ImGui::Button("Reset"))
                {
					transform.Position = glm::zero<glm::vec3>();
					transform.Rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
                }

                ImGui::SeparatorText("Render");

				if(ImGui::Checkbox("VSync", &m_vsyncEnabled))
				{
					g_Renderer->SetVSync(m_vsyncEnabled);
				}

                //if (ImGui.Checkbox("Wireframe", ref _wireframeEnabled))
                //{
                //    (Client.Render as OpenGLRender).WireframeEnabled = _wireframeEnabled;
                //}

                ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Misc"))
			{
				ImGui::Checkbox("Demo window", &m_demoViewEnabled);
				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}

		if (m_demoViewEnabled)
			ImGui::ShowDemoWindow();
	}

	ImGui::End();
}