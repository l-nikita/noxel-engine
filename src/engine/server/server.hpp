#pragma once

#include "singleton.hpp"

class Server : public Singleton<Server>
{
public:
	bool Init();
	void Update();
	void Shutdown();
};