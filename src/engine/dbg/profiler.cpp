#include <vector>
#include <string>

#include "profiler.hpp"
#include "debug.hpp"

namespace Profiler
{
    Profilers Profiler::m_all;

    Profiler::Profiler(const std::string& name, const std::string& category)
    {
        m_name = name;
        m_category = category;
        m_all[name] = this;
        m_start = std::chrono::high_resolution_clock::now();
    }

    void Profiler::Start()
    {
        m_start = std::chrono::high_resolution_clock::now();
    }

    void Profiler::End()
    {
        m_end = std::chrono::high_resolution_clock::now();
        m_lastFrameTime = std::chrono::duration<float, std::milli>(m_end - m_start).count();
    }

    void Start(const std::string& name, const std::string& category)
    {
#ifdef PROF_ENABLED
        auto it = Profiler::m_all.find(name);
        if (it == Profiler::m_all.end())
        {
            auto* p = new Profiler(name, category);
            p->Start();
        }
        else
            it->second->Start();
#endif
    }

    void End(const std::string& name)
    {
#ifdef PROF_ENABLED
        auto it = Profiler::m_all.find(name);
        if (it != Profiler::m_all.end())
            it->second->End();
        else
            FATAL_ERROR("Profiler [" + name + "] not found", CMD::ENGINE);
#endif
    }
}