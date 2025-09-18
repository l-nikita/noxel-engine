#include "client.hpp"
#include "dbg/debug.hpp"
#include "dbg/profiler.hpp"

//proto
#include "test.pb.h"

static void Client_OnConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* pInfo)
{
    Client::GetInstance().OnConnectionStatusChanged(pInfo);
}

bool Client::Start()
{
    m_interface = SteamNetworkingSockets();
    return true;
}

bool Client::Connect(const std::string& ip, uint16_t port)
{
    SteamNetworkingIPAddr serverAddr;
    serverAddr.Clear();
    serverAddr.ParseString(ip.c_str());
    serverAddr.m_port = port;

    CMD_INFO("Connecting to server at " + ip + ":" + std::to_string(port), CMD::CLIENT);

    SteamNetworkingConfigValue_t opt;
    opt.SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged, (void*)Client_OnConnectionStatusChanged);

    m_connection = m_interface->ConnectByIPAddress(serverAddr, 1, &opt);
    if (m_connection == k_HSteamNetConnection_Invalid)
    {
        CMD_ERROR("Failed to connect", CMD::CLIENT);
        return false;
    }

    m_isRunning = true;
    m_pollThread = std::thread(&Client::PollMessages, this);

    return true;
}

void Client::PollMessages()
{
    while (m_isRunning)
    {
        PROF_START("Client::PollMessages", "Client");
        ISteamNetworkingMessage* pIncomingMsg[10];
        int numMsgs = m_interface->ReceiveMessagesOnConnection(m_connection, pIncomingMsg, 10);
        for (int i = 0; i < numMsgs; ++i)
        {
            std::string msg((char*)pIncomingMsg[i]->m_pData, pIncomingMsg[i]->m_cbSize);
            CMD_INFO("Received: " + msg, CMD::CLIENT);
            pIncomingMsg[i]->Release();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        m_interface->RunCallbacks();
        PROF_END("Client::PollMessages");
    }
}

void Client::Send()
{
    NetworkMessage netMsg;
    auto* chat = netMsg.mutable_chat();
    chat->set_username("Player1");
    chat->set_message("Hello!");
    chat->set_timestamp(123456);

    //auto* pos = netMsg.mutable_position();
    //pos->set_username("SHREK");
    //pos->set_x(6);
    //pos->set_y(1);
    //pos->set_z(2);

    //auto* event = netMsg.mutable_event();
    //event->set_description("gonvno");
    //event->set_event_id(666);

    // Сериализовать
    std::string buffer;
    if (!netMsg.SerializeToString(&buffer)) 
        return;

    m_interface->SendMessageToConnection(m_connection, buffer.data(), buffer.size(), k_nSteamNetworkingSend_Reliable, nullptr);
}

void Client::OnConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* pInfo)
{
    switch (pInfo->m_info.m_eState)
    {
        case k_ESteamNetworkingConnectionState_Connecting:
            CMD_INFO("Connecting...", CMD::CLIENT);
            break;
        case k_ESteamNetworkingConnectionState_Connected:
            CMD_INFO("Connected!\n", CMD::CLIENT);
            break;
        case k_ESteamNetworkingConnectionState_ClosedByPeer:
        case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
            CMD_INFO("Connection closed\n", CMD::CLIENT);
            m_isRunning = false;
            break;
        default:
            break;
    }
}

void Client::Shutdown()
{
    m_isRunning = false;
    if (m_pollThread.joinable())
        m_pollThread.join();

    if (m_connection != k_HSteamNetConnection_Invalid)
    {
        m_interface->CloseConnection(m_connection, 0, "Client Shutdown", true);
        m_connection = k_HSteamNetConnection_Invalid;
    }

    CMD_INFO("Shutdown complete", CMD::CLIENT);
}