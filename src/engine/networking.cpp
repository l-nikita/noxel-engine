#include <steam/steamnetworkingsockets.h>
#include <steam/isteamnetworkingutils.h>

#include "networking.hpp"
#include "server.hpp"
#include "client.hpp"
#include "cmd.hpp"

static void SteamNetDebugOutput(ESteamNetworkingSocketsDebugOutputType eType, const char* pszMsg)
{
    if (eType == k_ESteamNetworkingSocketsDebugOutputType_Bug || eType == k_ESteamNetworkingSocketsDebugOutputType_Error)
        CMD_LOG(pszMsg, ERROR);
    else if (eType == k_ESteamNetworkingSocketsDebugOutputType_Warning || eType == k_ESteamNetworkingSocketsDebugOutputType_Important)
        CMD_LOG(pszMsg, WARNING);
    else
        CMD_LOG(pszMsg, INFO);
}

bool Networking::Init()
{
    SteamDatagramErrMsg errorMsg;
    if (!GameNetworkingSockets_Init(nullptr, errorMsg)) 
    {
        CMD_LOG("GNS Init failed: " + std::string(errorMsg), ERROR);
        return false;
    }

    SteamNetworkingUtils()->SetDebugOutputFunction(k_ESteamNetworkingSocketsDebugOutputType_Msg, SteamNetDebugOutput);

    CMD::GetInstance().RegisterCommand("net_start", "", [this]() {
        Start();
    });

    CMD::GetInstance().RegisterCommand("net_connect_local", "", [this]() {
        if (!m_isRunning && !Client::GetInstance().IsConnected()) return;
        Client::GetInstance().Connect("127.0.0.1", 27015);
    });

    CMD::GetInstance().RegisterCommand("cl_test_msg", "", [this]() {
        if (!m_isRunning && !Client::GetInstance().IsConnected()) return;
        Client::GetInstance().Send();
    });

    return true;
}

bool Networking::Start()
{
    if (m_isRunning) return false;

    CMD_LOG("//------------------------------------------", DEFAULT);
    CMD_LOG("] Starting networking...", INFO);
    CMD_LOG("//------------------------------------------", DEFAULT);

    if (!Server::GetInstance().Start()) 
        return false;

    if (!Client::GetInstance().Start()) 
        return false;

    m_isRunning = true;

    return true;
}

void Networking::Shutdown()
{
    if (!m_isRunning) return;

    m_isRunning = false;
    Server::GetInstance().Shutdown();
    Client::GetInstance().Shutdown();
}