#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include "dev_menu.hpp"
#include "renderer.hpp"

void DevPanel::Draw()
{
	if (ImGui::Begin("Developer menu"))
	{
		if (ImGui::BeginTabBar("MyTabBar"))
		{
			if (ImGui::BeginTabItem("General"))
			{
                ImGui::SeparatorText("Camera");

                auto& transform = Renderer::GetInstance().m_Camera->m_Transform;
                ImGui::InputFloat3("Position", glm::value_ptr(transform.Position) );

                glm::vec3 ang(transform.Rotation.Pitch, transform.Rotation.Yaw, transform.Rotation.Roll);
                ImGui::InputFloat3("Rotation", glm::value_ptr(ang));

                if (ImGui::Button("Reset"))
                {
					transform.Position = glm::zero<glm::vec3>();
					transform.Rotation = Angles::Zero();
                }

                ImGui::SeparatorText("Render");

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