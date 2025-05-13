#include <iostream>
#include "engine.hpp"

int main() 
{
	if (Engine::GetInstance().Init())
		Engine::GetInstance().Run();

	Engine::GetInstance().Stop();

	return 0;
}