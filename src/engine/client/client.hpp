#pragma once

#include <thread>
#include <iostream>
#include <steam/steamnetworkingsockets.h>
#include <steam/steamnetworkingtypes.h>
#include <steam/isteamnetworkingutils.h>

#include "singleton.hpp"

class Client : public Singleton<Client>
{
public:
    bool Start();
    bool Connect(const std::string& ip, uint16_t port);
    void PollMessages();
    void Shutdown();
    void Send();

    void OnConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* pInfo);

    bool IsConnected() { return m_isRunning; }

private:
    ISteamNetworkingSockets* m_interface = nullptr;
    HSteamNetConnection m_connection = k_HSteamNetConnection_Invalid;
    std::atomic_bool m_isRunning = false;
    std::thread m_pollThread;
};