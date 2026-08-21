#pragma once

#include <utility>
#include <source_location>
#include <SDL3/SDL.h>

#include "profiler.hpp"
#include "cmd/cmd.hpp"

namespace debug
{
#if DEBUG

#if _MSC_VER
#define BREAKPOINT() \
    __debugbreak()
#elif __clang__ || __GNUC__
#define BREAKPOINT() \
    __builtin_trap()
#else
#define BREAKPOINT()
#endif


#endif

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
        cmd::LogCategory category,
        cmd::LogType type )
    {
        cmd::CMD::Get().LogMsg(
            std::string( msg ),
            type,
            category
        );
    }

    inline void FatalError( const std::string& message, cmd::LogCategory category,
                            const std::source_location& location = std::source_location::current() )
    {
        const auto now = std::chrono::system_clock::now();
        auto localTime = std::chrono::current_zone()->to_local( now );

        auto formatedMsg = message + "\n";

        formatedMsg += "\nSource: [" + std::string( location.file_name() ) + ":" + std::to_string( location.line() ) +
                "]";
        formatedMsg += std::format(
            "\nTime: {:%F %T}",
            localTime
        );

        formatedMsg += "\n";
        formatedMsg += GetSystemDebugInfo();

        cmd::CMD::Get().LogMsg(
            formatedMsg,
            cmd::FATAL_ERROR,
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

#if DEBUG
        BREAKPOINT();
#endif
        std::exit( EXIT_FAILURE );
    }
}

//-----------------------------------------------------------------------------
// Macros
//-----------------------------------------------------------------------------

#define CMD_LOG(msg, type, category) \
    ::debug::Log((msg), type, category)

#define CMD_MSG(msg, category) \
    ::debug::Log((msg), category, cmd::DEFAULT)

#define CMD_DMSG(msg) \
    ::debug::Log((msg), cmd::NONE, cmd::DEFAULT)

#define CMD_INFO(msg, category) \
    ::debug::Log((msg), category, cmd::INFO)

#define CMD_WARNING(msg, category) \
    ::debug::Log((msg), category, cmd::WARNING)

#define CMD_ERROR(msg, category) \
    ::debug::Log((msg), category, cmd::ERROR)

#define FATAL_ERROR(msg, category) \
    ::debug::FatalError((msg), category)

//-----------------------------------------------------------------------------

#define PROF_START(name, category) \
    profiler::Start(name, category);

#define PROF_END(name) \
    profiler::End(name);
