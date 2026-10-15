#include <mimalloc.h>

#include "engine.hpp"

// TODO: make args (command line)

int main()
{
    (void)mi_version();

    g_Engine = new Engine();
    g_Engine->Initialize();
    g_Engine->Run();

    delete g_Engine;
    g_Engine = nullptr;
}
