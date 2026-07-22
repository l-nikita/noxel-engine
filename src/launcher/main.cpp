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
    if ( !SetConsoleCtrlHandler( ConsoleHandler, TRUE ) )
    {
        std::cerr << "Error: Couldn't set control handler for console!" << std::endl;
        return 1;
    }
#endif

    g_Engine = new Engine();
    g_Engine->Initialize();
    g_Engine->Run();

    delete g_Engine;
    g_Engine = nullptr;
}
