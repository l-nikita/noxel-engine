#include <imgui.h>

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

                //auto pos = Renderer::GetInstance().m_Camera;
                //ImGui::InputFloat3("Position", ref pos);

                //var ang = new Vector3(
                //    Camera.Transform.Rotation.Pitch,
                //    Camera.Transform.Rotation.Yaw,
                //    Camera.Transform.Rotation.Roll);

                //ImGui::InputFloat3("Rotation", ref ang);

                if (ImGui::Button("Reset"))
                {
                    //Camera.Transform.Position = Vector3.Zero;
                    //Camera.Transform.Rotation = Angles.Zero;
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
	}

	ImGui::End();
}