#include "imgui.h"
#include "console_panel.hpp"
#include "cmd/cmd.hpp"

void ConsolePanel::Draw()
{
    if (ImGui::Begin("Console", NULL, (ImGuiWindowFlags_MenuBar)))
    {
        DrawMenuBar();
        DrawLogWindow();
        ImGui::Separator();
        DrawInput();
    }

    ImGui::End();
}

void ConsolePanel::DrawMenuBar()
{
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Settings"))
        {
            ImGui::Checkbox("Time stamps", &m_timeStamp);
            ImGui::Checkbox("Auto scroll", &m_autoScroll);
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }
}

void ConsolePanel::DrawLogWindow()
{
    float footerHeightToReserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
    if (ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footerHeightToReserve)))
    {
        const float timestampWidth = ImGui::CalcTextSize("00:00:00").x;

        ImGui::PushTextWrapPos();

        auto& logs = CMD::GetInstance().GetLogs();
        for (auto it = logs.begin(); it != logs.end(); ++it)
        {
            auto* log = it->get();
            ImGui::PushTextWrapPos(ImGui::GetColumnWidth() - timestampWidth);

            ImVec4 color;
            switch (log->Type) 
            {
                case CMDLogType::INFO:    color = ImVec4(0.4f, 1.0f, 1.0f, 1.0f); break;
                case CMDLogType::WARNING: color = ImVec4(0.9f, 0.7f, 0.0f, 1.0f); break;
                case CMDLogType::ERROR:   color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); break;
                default:                  color = ImVec4(0.8f, 0.8f, 0.8f, 1.0f); break;
            }

            ImGui::PushStyleColor(ImGuiCol_Text, color);
            ImGui::Text(log->Text.c_str());
            ImGui::PopStyleColor();

            if (m_timeStamp)
            {
                ImGui::PopTextWrapPos();
                ImGui::SameLine(ImGui::GetColumnWidth() - timestampWidth);

                std::tm* timeInfo = std::localtime(&log->Time);
                char timeBuffer[32];
                std::strftime(timeBuffer, sizeof(timeBuffer), "%H:%M:%S", timeInfo);

                ImGui::PushStyleColor(ImGuiCol_Text, color);
                ImGui::Text(timeBuffer);
                ImGui::PopStyleColor();
            }
        }

        ImGui::PopTextWrapPos();

        if (m_scrollToBottom)
        {
            ImGui::SetScrollHereY(1.0f);
            m_scrollToBottom = false;
        }
    }

    ImGui::EndChild();
}

void ConsolePanel::DrawInput()
{
    if (ImGui::Button("Clear"))
        CMD::GetInstance().Execute("clear");

    ImGui::SameLine();

    bool reclaimFocus = false;

    ImGui::PushItemWidth(-ImGui::GetStyle().ItemSpacing.x * 1);

    if (ImGui::InputText("##", m_input, INPUT_SIZE, ImGuiInputTextFlags_EnterReturnsTrue))
    {
        reclaimFocus = true;
        ExecuteCommand(m_input);
    }

    ImGui::PopItemWidth();

    ImGui::SetItemDefaultFocus();
    if (reclaimFocus)
        ImGui::SetKeyboardFocusHere(-1);
}

void ConsolePanel::ExecuteCommand(char* input)
{
    if (input == nullptr) return;

    std::string str = input;
    if (str.empty()) return;

    if (m_autoScroll)
        m_scrollToBottom = true;

    CMD::GetInstance().Execute(str);

    memset(input, 0, INPUT_SIZE);
}