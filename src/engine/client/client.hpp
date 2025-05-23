#pragma once

#include <iostream>
#include "singleton.hpp"

class Client : public Singleton<Client>
{
public:
	bool Init();
	bool Connect(const std::string& ip, uint16_t port);
	void Update();
	void Shutdown();
};