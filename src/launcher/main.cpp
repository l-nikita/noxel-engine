#include <iostream>
#include "engine.hpp"

#ifdef _WIN32
#include <windows.h>

    BOOL WINAPI ConsoleHandler( DWORD signal )
{
    if ( signal == CTRL_C_EVENT || signal == CTRL_CLOSE_EVENT )
        return TRUE;

    return FALSE;
}
#endif

// TODO: make args (command line)

int main()
{
#ifdef _WIN32
    if ( !SetConsoleCtrlHandler(ConsoleHandler, TRUE) )
    {
        std::cerr << "Error: Couldn't set control handler for console!" << std::endl;
        return 1;
    }
#endif

    g_Engine = new Engine();

    if ( g_Engine->Init() )
        g_Engine->Run();
    else
        std::cerr << "Error: Couldn't init engine!!!" << std::endl;

    delete g_Engine;

    return 0;
}
