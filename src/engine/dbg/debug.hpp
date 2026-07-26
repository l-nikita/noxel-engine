#pragma once

#include <source_location>
#include <SDL3/SDL.h>
#include "profiler.hpp"
#include "cmd/cmd.hpp"

namespace Debug
{
    inline std::string GetSystemDebugInfo()
    {
        std::string info;

        info += "\nPlatform: ";
        info += SDL_GetPlatform();

        info += "\nCPU Logical Cores: ";
        info += std::to_string( SDL_GetNumLogicalCPUCores() );

        info += "\nRAM: ";
        info += std::to_string( SDL_GetSystemRAM() );

        return info;
    }

    inline void Log(
        std::string_view msg,
        CMD::LogCategory category,
        CMD::LogType type )
    {
        CMD::CMD::Get().LogMsg(
            std::string( msg ),
            type,
            category
        );
    }

    inline void FatalError( const std::string& message, CMD::LogCategory category,
                            const std::source_location& location = std::source_location::current() )
    {
        const auto now = std::chrono::system_clock::now();
        auto localTime = std::chrono::current_zone()->to_local( now );

        auto formatedMsg = "Error: " + message + "\n";

        formatedMsg += "\nSource: [" + std::string( location.file_name() ) + ":" + std::to_string( location.line() ) +
                "]";
        formatedMsg += std::format(
            "\nTime: {:%F %T}",
            localTime
        );

        formatedMsg += "\n";
        formatedMsg += GetSystemDebugInfo();

        CMD::CMD::Get().LogMsg(
            formatedMsg,
            CMD::FATAL_ERROR,
            category,
            now
        );

        std::string title = std::string( ENGINE_NAME ) + " - Fatal Error";

        SDL_ShowSimpleMessageBox(
            SDL_MESSAGEBOX_ERROR,
            title.c_str(),
            formatedMsg.c_str(),
            nullptr
        );

        throw std::runtime_error( message );
    }
}

//-----------------------------------------------------------------------------
// Macros
//-----------------------------------------------------------------------------

#define CMD_LOG(msg, type, category) \
    ::Debug::Log((msg), type, category)

#define CMD_MSG(msg, category) \
    ::Debug::Log((msg), category, CMD::DEFAULT)

#define CMD_DMSG(msg) \
    ::Debug::Log((msg), CMD::NONE, CMD::DEFAULT)

#define CMD_INFO(msg, category) \
    ::Debug::Log((msg), category, CMD::INFO)

#define CMD_WARNING(msg, category) \
    ::Debug::Log((msg), category, CMD::WARNING)

#define CMD_ERROR(msg, category) \
    ::Debug::Log((msg), category, CMD::ERROR)

#define FATAL_ERROR(msg, category) \
    ::Debug::FatalError((msg), category)

//-----------------------------------------------------------------------------

#define PROF_START(name, category) \
    Profiler::Start(name, category);

#define PROF_END(name) \
    Profiler::End(name);
