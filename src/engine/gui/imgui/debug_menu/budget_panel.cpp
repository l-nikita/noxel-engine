#include <imgui.h>

#include "engine.hpp"
#include "renderer/renderer.hpp"
#include "budget_panel.hpp"
#include "common/profiler.hpp"

static ImGuiTableFlags table_flags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH
	| ImGuiTableFlags_Resizable
	| ImGuiTableFlags_RowBg
	| ImGuiTableFlags_NoBordersInBody;

void BudgetPanel::Draw()
{
	if (ImGui::Begin("Budget"))
	{
		auto& io = ImGui::GetIO();
		m_fps = io.Framerate;
		ImGui::Text("FPS: %f", m_fps);
#ifdef _DEBUG
		ImGui::Text("DrawCalls: %i", g_Renderer->GetDrawCallCount());
#endif
		ImGui::Separator();

		if (ImGui::BeginTable("ProfilersTable", 2, table_flags))
		{
			ImGui::TableSetupColumn("Name");
			ImGui::TableSetupColumn("Time");

			for (auto profiler : Profiler::All)
				DrawRow(profiler);

			ImGui::EndTable();
		}

		ImGui::Separator();
		ImGui::PlotLines("FPS", m_frameRates, FPS_HISTORY_SIZE, m_frameRateIndex, NULL, 0.0f, 300.0f, ImVec2(300, 80));

		m_frameRates[m_frameRateIndex] = m_fps;
		if (m_frameRateIndex < FPS_HISTORY_SIZE - 1)
			m_frameRateIndex++;
		else
			m_frameRateIndex = 0;
	}

	ImGui::End();
}

void BudgetPanel::DrawRow(Profiler* profiler)
{
	if (profiler == nullptr) return;

	ImGui::TableNextRow();

	for (int column = 0; column < 2; column++)
	{
		ImGui::TableSetColumnIndex(column);
		if (column == 0)
			ImGui::Text("%s", profiler->GetName().c_str());
		if (column == 1)
			ImGui::Text("%.4f ms", profiler->GetTime());
	}
}