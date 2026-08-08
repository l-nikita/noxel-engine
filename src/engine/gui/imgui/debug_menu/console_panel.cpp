#include <algorithm>
#include <format>
#include <ranges>
#include <string>
#include <vector>
#include <imgui.h>

#include "console_panel.hpp"
#include "dbg/debug.hpp"

void ConsolePanel::Draw()
{
    if ( ImGui::Begin( "Console", nullptr, ( ImGuiWindowFlags_MenuBar ) ) )
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
    if ( ImGui::BeginMenuBar() )
    {
        if ( ImGui::BeginMenu( "Settings" ) )
        {
            ImGui::Checkbox( "Time stamps", &m_timeStamp );
            ImGui::Checkbox( "Auto scroll", &m_autoScroll );
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }
}

void ConsolePanel::DrawLogWindow()
{
    const float footerHeightToReserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
    if ( ImGui::BeginChild( "ScrollingRegion", ImVec2( 0, -footerHeightToReserve ) ) )
    {
        const float timestampWidth = ImGui::CalcTextSize( "00:00:00" ).x;

        ImGui::PushTextWrapPos();

        auto& logs = cmd::CMD::Get().GetLogs();
        for ( auto log = logs.begin(); log != logs.end(); ++log )
        {
            ImGui::PushTextWrapPos( ImGui::GetColumnWidth() - timestampWidth );

            ImVec4 color;
            switch ( log->Type )
            {
                case cmd::LogType::INFO: color = ImVec4( 0.4f, 1.0f, 1.0f, 1.0f );
                    break;
                case cmd::LogType::WARNING: color = ImVec4( 0.9f, 0.7f, 0.0f, 1.0f );
                    break;
                case cmd::LogType::ERROR: color = ImVec4( 1.0f, 0.0f, 0.0f, 1.0f );
                    break;
                default: color = ImVec4( 0.8f, 0.8f, 0.8f, 1.0f );
                    break;
            }

            ImGui::PushStyleColor( ImGuiCol_Text, color );

            std::string formated;
            if ( log->Category != cmd::CONSOLE )
                formated = std::format( "[{}] {}\n", cmd::LogCategoryToString( log->Category ), log->Text );
            else
                formated = log->Text;

            ImGui::Text( "%s", formated.c_str() );
            ImGui::PopStyleColor();

            if ( m_timeStamp )
            {
                ImGui::PopTextWrapPos();
                ImGui::SameLine( ImGui::GetColumnWidth() - timestampWidth );

                const auto timeInfo = std::localtime( &log->Time );
                char timeBuffer[ 32 ];
                std::strftime( timeBuffer, sizeof( timeBuffer ), "%H:%M:%S", timeInfo );

                ImGui::PushStyleColor( ImGuiCol_Text, color );
                ImGui::Text( "%s", timeBuffer );
                ImGui::PopStyleColor();
            }
        }

        ImGui::PopTextWrapPos();

        if ( m_scrollToBottom )
        {
            ImGui::SetScrollHereY( 1.0f );
            m_scrollToBottom = false;
        }
    }

    ImGui::EndChild();
}

void ConsolePanel::DrawInput()
{
    if ( ImGui::Button( "Clear" ) )
        cmd::CMD::Get().Execute( "clear" );

    ImGui::SameLine();

    bool reclaimFocus = false;
    bool suggestionAccepted = false;

    ImGui::PushItemWidth( -1.0f );

    const bool inputSubmitted =
            ImGui::InputText(
                "##Input",
                m_buffer,
                INPUT_SIZE,
                ImGuiInputTextFlags_EnterReturnsTrue
            );

    const bool inputIsActive = ImGui::IsItemActive();
    const bool inputEdited = ImGui::IsItemEdited();
    const ImVec2 inputMin = ImGui::GetItemRectMin();
    const ImVec2 inputMax = ImGui::GetItemRectMax();

    m_input.assign( m_buffer );

    if ( inputEdited )
        m_selectedSuggestion = -1;

    std::vector<std::string> matches = FindMatches( m_input );

    if ( m_selectedSuggestion >= static_cast<int>(matches.size()) )
        m_selectedSuggestion = -1;

    bool selectionMoved = false;

    if ( inputIsActive && !matches.empty() )
    {
        if ( ImGui::IsKeyPressed( ImGuiKey_DownArrow ) )
        {
            m_selectedSuggestion =
                    ( m_selectedSuggestion + 1 ) % static_cast<int>(matches.size());

            selectionMoved = true;
        } else if ( ImGui::IsKeyPressed( ImGuiKey_UpArrow ) )
        {
            m_selectedSuggestion =
                    m_selectedSuggestion <= 0
                        ? static_cast<int>(matches.size()) - 1
                        : m_selectedSuggestion - 1;

            selectionMoved = true;
        }

        ImGui::OpenPopup( "##SuggestionsPopup" );
    }

    if ( inputSubmitted && m_selectedSuggestion >= 0 )
    {
        m_input = matches[ static_cast<std::size_t>(m_selectedSuggestion) ];

        std::snprintf( m_buffer, INPUT_SIZE, "%s", m_input.c_str() );

        m_selectedSuggestion = -1;
        suggestionAccepted = true;
        reclaimFocus = true;
    }

    ImGui::SetNextWindowPos( ImVec2( inputMin.x, inputMax.y ) );
    ImGui::SetNextWindowSize(
        ImVec2( inputMax.x - inputMin.x, 0.0f )
    );

    constexpr ImGuiWindowFlags popupFlags =
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoFocusOnAppearing;

    if ( ImGui::BeginPopup( "##SuggestionsPopup", popupFlags ) )
    {
        if ( matches.empty() || suggestionAccepted )
            ImGui::CloseCurrentPopup();
        else
        {
            for ( std::size_t i = 0; i < matches.size(); ++i )
            {
                const bool selected =
                        static_cast<int>(i) == m_selectedSuggestion;

                if ( ImGui::Selectable( matches[ i ].c_str(), selected ) )
                {
                    m_input = matches[ i ];

                    std::snprintf(
                        m_buffer,
                        INPUT_SIZE,
                        "%s",
                        m_input.c_str()
                    );

                    m_selectedSuggestion = -1;
                    suggestionAccepted = true;
                    reclaimFocus = true;

                    ImGui::CloseCurrentPopup();
                }

                if ( selectionMoved && selected )
                    ImGui::SetScrollHereY();
            }
        }

        ImGui::EndPopup();
    }

    if ( inputSubmitted && !suggestionAccepted )
    {
        reclaimFocus = true;

        if ( !m_input.empty() )
        {
            if ( m_autoScroll )
                m_scrollToBottom = true;

            cmd::Execute( m_input );
        }

        m_input.clear();
        m_buffer[ 0 ] = '\0';
        m_selectedSuggestion = -1;

        ImGui::CloseCurrentPopup();
    }

    ImGui::PopItemWidth();

    if ( reclaimFocus )
        ImGui::SetKeyboardFocusHere( -1 );
}

std::vector<std::string> ConsolePanel::FindMatches( const std::string& input )
{
    std::vector<std::string> matches;
    if ( input.empty() )
        return matches;

    const auto addMatch = [&matches, input]( const std::string& name )
    {
        if ( name != input && name.contains( input ) )
            matches.push_back( name );
    };

    for ( const auto& name: cmd::CMD::Get().GetCommands() | std::views::keys )
        addMatch( name );

    for ( const auto& name: cmd::CMD::Get().GetVars() | std::views::keys )
        addMatch( name );

    std::ranges::sort( matches );
    matches.erase( std::ranges::unique( matches ).begin(), matches.end() );

    return matches;
}
