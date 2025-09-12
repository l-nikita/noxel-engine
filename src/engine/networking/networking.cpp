#include <GameNetworkingSockets/steam/steamnetworkingsockets.h>
#include <GameNetworkingSockets/steam/isteamnetworkingutils.h>

#include "networking.hpp"
#include "server/server.hpp"
#include "client/client.hpp"
#include "dbg/debug.hpp"

static void SteamNetDebugOutput(ESteamNetworkingSocketsDebugOutputType eType, const char* pszMsg)
{
    if (eType == k_ESteamNetworkingSocketsDebugOutputType_Bug || eType == k_ESteamNetworkingSocketsDebugOutputType_Error)
        CMD_ERROR(pszMsg, CMD::NETWORKING);
    else if (eType == k_ESteamNetworkingSocketsDebugOutputType_Warning || eType == k_ESteamNetworkingSocketsDebugOutputType_Important)
        CMD_WARNING(pszMsg, CMD::NETWORKING);
    else
        CMD_INFO(pszMsg, CMD::NETWORKING);
}

bool Networking::Init()
{
    SteamDatagramErrMsg errorMsg;
    if (!GameNetworkingSockets_Init(nullptr, errorMsg)) 
    {
        CMD_ERROR("GNS Init failed: " + std::string(errorMsg), CMD::NETWORKING);
        return false;
    }

    SteamNetworkingUtils()->SetDebugOutputFunction(k_ESteamNetworkingSocketsDebugOutputType_Msg, SteamNetDebugOutput);

    CMD::RegisterCommand("net_start", "", [this]() {
        Start();
    });

    CMD::RegisterCommand("net_connect_local", "", [this]() {
        if (!m_isRunning && !Client::GetInstance().IsConnected()) return;
        Client::GetInstance().Connect("127.0.0.1", 27015);
    });

    CMD::RegisterCommand("cl_test_msg", "", [this]() {
        if (!m_isRunning && !Client::GetInstance().IsConnected()) return;
        Client::GetInstance().Send();
    });

    CMD_INFO("] Networking initialized", CMD::NETWORKING);
    return true;
}

bool Networking::Start()
{
    if (m_isRunning) return false;

    CMD_DMSG("//------------------------------------------");
    CMD_INFO("] Starting networking...", CMD::NETWORKING);
    CMD_DMSG("//------------------------------------------");

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