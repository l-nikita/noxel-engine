#include <GameNetworkingSockets/steam/isteamnetworkingsockets.h>
#include <GameNetworkingSockets/steam/steamnetworkingsockets.h>
#include <GameNetworkingSockets/steam/steamnetworkingtypes.h>
#include <GameNetworkingSockets/steam/isteamnetworkingutils.h>
#include <GameNetworkingSockets/steam/isteamnetworkingmessages.h>

#include <atomic>
#include <thread>

#include "server.hpp"
#include "cmd.hpp"
#include "profiler.hpp"

//proto
#include "test.pb.h"

static void Server_OnConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* pInfo)
{
    Server::GetInstance().OnConnectionStatusChanged(pInfo);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool Server::Start()
{
    CMD_LOG_INFO("[Server] Starting...");

    if (!StartNet()) 
        return false;

    m_tickThread = std::thread(&Server::TickLoop, this);

    return true;
}

void Server::TickLoop()
{
    constexpr double tickRate = 20.0; // 20 ticks/sec
    constexpr double tickInterval = 1.0 / tickRate;

    using namespace std::chrono;
    using clock = high_resolution_clock;
    auto nextTick = clock::now();

    while (m_isRunning)
    {
        auto now = clock::now();

        if (now >= nextTick)
        {
            Tick();
            nextTick += duration_cast<steady_clock::duration>(duration<double>(tickInterval));
        }
        else
            std::this_thread::sleep_for(nextTick - now);
    }
}

void Server::Tick()
{

}

//-----------------------------------------------------------------------------
// Networking
//-----------------------------------------------------------------------------
bool Server::StartNet()
{
    m_interface = SteamNetworkingSockets();

    SteamNetworkingIPAddr serverAddr;
    serverAddr.Clear();
    serverAddr.m_port = 27015;

    SteamNetworkingConfigValue_t opt;
    opt.SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged, (void*)Server_OnConnectionStatusChanged);

    m_listenSock = SteamNetworkingSockets()->CreateListenSocketIP(serverAddr, 1, &opt);
    if (m_listenSock == k_HSteamListenSocket_Invalid)
    {
        CMD_LOG("[Server] Failed to create listen socket", ERROR);
        return false;
    }

    CMD_LOG_INFO("[Server] Start listening on port 27015");

    m_pollGroup = m_interface->CreatePollGroup();

    m_isRunning = true;
    m_msgThread = std::thread(&Server::PollMessages, this);

    return true;
}

void Server::PollMessages()
{
    Profiler msgProfiler("[Server] PollMessages");
    while (m_isRunning)
    {
        msgProfiler.Start();
        ISteamNetworkingMessage* msg = nullptr;

        int numMsgs = m_interface->ReceiveMessagesOnPollGroup(m_pollGroup, &msg, 1);
        if (numMsgs > 0 && msg)
        {
            ProcessMessage(msg);
            msg->Release();
        }
        else
            std::this_thread::sleep_for(std::chrono::milliseconds(10));

        m_interface->RunCallbacks();
        msgProfiler.End();
    }
}

void Server::ProcessMessage(ISteamNetworkingMessage* msg)
{
    NetworkMessage netMsg;
    if (!netMsg.ParseFromArray(msg->m_pData, msg->m_cbSize)) 
    {
        CMD_LOG("[Server] Error when parsing net message", WARNING);
        return;
    }

    switch (netMsg.payload_case()) 
    {
        case NetworkMessage::kChat:
        {
            const auto& chat = netMsg.chat();
            CMD_LOG("[Server] username " + chat.username(), DEFAULT);
            CMD_LOG("[Server] message " + chat.message(), DEFAULT);
            CMD_LOG("[Server] timestamp " + std::to_string(chat.timestamp()), DEFAULT);
            break;
        }
        case NetworkMessage::kPosition:
        {
            const auto& pos = netMsg.position();
            CMD_LOG("[Server] username " + pos.username(), DEFAULT);
            CMD_LOG("[Server] x " + std::to_string(pos.x()), DEFAULT);
            CMD_LOG("[Server] y " + std::to_string(pos.y()), DEFAULT);
            CMD_LOG("[Server] z " + std::to_string(pos.z()), DEFAULT);
            break;
        }
        case NetworkMessage::kEvent:
        {
            const auto& event = netMsg.event();
            CMD_LOG("[Server] description " + event.description(), DEFAULT);
            CMD_LOG("[Server] event_id " + std::to_string(event.event_id()), DEFAULT);
            break;
        }
        default:
        {
            CMD_LOG("[Server] Unknown message type", WARNING);
            break;
        }
    }
}

void Server::OnConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* pInfo)
{
    switch (pInfo->m_info.m_eState)
    {
        case k_ESteamNetworkingConnectionState_Connecting:
            SteamNetworkingSockets()->AcceptConnection(pInfo->m_hConn);
            SteamNetworkingSockets()->SetConnectionPollGroup(pInfo->m_hConn, m_pollGroup);
            m_clients.emplace(pInfo->m_hConn, Client("Unknown"));
            CMD_LOG("[Server] New client connected", INFO);
            break;

        case k_ESteamNetworkingConnectionState_Connected:
            break;

        case k_ESteamNetworkingConnectionState_ClosedByPeer:
        case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
            m_clients.erase(pInfo->m_hConn);
            SteamNetworkingSockets()->CloseConnection(pInfo->m_hConn, 0, nullptr, false);
            CMD_LOG("[Server] Client disconnected", INFO);
            break;

        default:
            break;
    }
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Server::Shutdown()
{
    m_isRunning = false;

    if (m_msgThread.joinable())
        m_msgThread.join();

    if (m_tickThread.joinable())
        m_tickThread.join();

    for (auto& c : m_clients)
        m_interface->CloseConnection(c.first, 0, "Shutdown", true);

    m_clients.clear();

    m_interface->CloseListenSocket(m_listenSock);
    m_interface->DestroyPollGroup(m_pollGroup);

    CMD_LOG_INFO("[Server] Shutdown complete");
}