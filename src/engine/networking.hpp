#pragma once

#include "singleton.hpp"

class Networking : public Singleton<Networking>
{
public:
	bool Init();
	bool Start();
	void Shutdown();

	bool IsRunning() { return m_isRunning; }

private:
	bool m_isRunning;
};