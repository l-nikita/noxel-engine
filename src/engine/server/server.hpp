#pragma once

#include <map>
#include <string>
#include <thread>
#include <GameNetworkingSockets/steam/steamnetworkingtypes.h>
#include <GameNetworkingSockets/steam/isteamnetworkingsockets.h>

#include "common/singleton.hpp"

class Server : public Singleton<Server>
{
public:
	bool Start();
	bool StartNet();

	void TickLoop();
	void Tick();

	// networking
	void PollMessages();
	void ProcessMessage(ISteamNetworkingMessage* msg);
	void Shutdown();

	void OnConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* pInfo);

protected:
	std::atomic_bool m_isRunning{ false };
	std::thread m_tickThread;

	// networking
	std::thread m_msgThread;

	HSteamListenSocket m_listenSock{};
	HSteamNetPollGroup m_pollGroup{};
	ISteamNetworkingSockets* m_interface{ nullptr };

	struct Client
	{
		std::string Nick;
		Client(const std::string& nick) : Nick(nick) {}
	};

	std::map<HSteamNetConnection, Client> m_clients;
};