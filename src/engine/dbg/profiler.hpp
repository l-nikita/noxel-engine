#pragma once

#include <iostream>
#include <chrono>
#include <vector>
#include <string>
#include <unordered_map>
#include <cassert>

#include "common.hpp"

#define PROF_ENABLED _DEBUG

namespace Profiler
{
    class Profiler;
    using Profilers = std::unordered_map<std::string, Profiler*>;

    class ENGINE_API Profiler
    {
    public:
        static Profilers m_all;
        Profiler(const std::string& name, const std::string& category);

    private:
        std::string m_name;
        std::string m_category;
        std::chrono::high_resolution_clock::time_point m_start;
        std::chrono::high_resolution_clock::time_point m_end;
        float m_lastFrameTime = 0.0f;

    public:
        void Start();
        void End();

        float GetTime() const { return m_lastFrameTime; }
        std::string GetName() const { return m_name; }
        std::string GetCategory() const { return m_category; }
    };

    Profilers GetAll();

    void Start(const std::string& name, const std::string& category);
    void End(const std::string& name);
}