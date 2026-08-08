#pragma once

#include <string>
#include <ctime>

#ifdef ERROR
#undef ERROR
#endif

namespace cmd
{
    enum LogType
    {
        DEFAULT, // White
        INFO, // Blue
        WARNING, // Orange
        ERROR, // Red
        FATAL_ERROR,
    };

    enum LogCategory
    {
        NONE = 0,
        ENGINE,
        SERVER,
        CLIENT,
        NETWORKING,
        TOOLS,
        PHYSICS,
        RENDERING,
        ASSETSYSTEM,
        FILESYSTEM,
        CONSOLE,
        INPUT,
    };

    std::string LogTypeToString( LogType type );
    std::string LogCategoryToString( LogCategory cat );

    struct Log
    {
        const std::string Text;
        const std::time_t Time;
        const LogType Type;
        const LogCategory Category;

        Log( const std::string& text, std::time_t time, LogType type, LogCategory cat )
            : Text( text ), Time( time ), Type( type ), Category( cat )
        {
        }
    };
}
