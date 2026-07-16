#pragma once

#include <chrono>
#include <functional>
#include <optional>
#include <string>
#include <vector>

#include "cmd_command.hpp"
#include "cmd_log.hpp"
#include "cmd_var.hpp"
#include "common/singleton.hpp"

namespace CMD
{
    using Commands = std::unordered_map<std::string, Command>;
    using Vars = std::unordered_map<std::string, Var>;
    using Logs = std::vector<Log>;
    using TimePoint = std::chrono::system_clock::time_point;

    class CMD : public Singleton<CMD>
    {
    public:
        void Setup();

        void RegisterCommand( const std::string& name, const std::string& desc, const std::function<void()>& func );
        bool IsCommandExist( const std::string& name );
        void Execute( const std::string& name );

        void RegisterVar( const std::string& name, const std::string& desc, const VarValue& defValue,
                          const VarCallback& callback = nullptr );

        bool IsVarExist( const std::string& name );
        void SetVar( const std::string& name, const VarValue& value );
        std::optional<VarValue> GetVarValue( const std::string& name ) const;

        Commands& GetCommands();
        Vars& GetVars();
        Logs& GetLogs();

        static std::vector<std::string> Tokenize( const std::string& input );
        static VarValue ConvertStringToVarValue( const std::string& input );

        void LogMsg( const std::string& msg, LogType type, LogCategory category,
                     TimePoint time = std::chrono::system_clock::now() );
        void ClearLogs();

        void LogHelp();

    private:
        Logs m_logs;
        Commands m_commands;
        Vars m_vars;
    };

    void RegisterCommand( const std::string& name, const std::string& desc, const std::function<void()>& func );
    void RegisterVar( const std::string& name, const std::string& desc, const VarValue& defValue,
                      const VarCallback& callback = nullptr );
    void Execute( const std::string& name );
}
