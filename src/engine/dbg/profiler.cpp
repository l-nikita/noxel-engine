#include <memory>
#include <vector>
#include <string>

#include "profiler.hpp"
#include "debug.hpp"

namespace profiler
{
    Profilers Profiler::m_all;

    Profiler::Profiler( const std::string& name, const std::string& category )
    {
        m_name = name;
        m_category = category;
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

    const Profilers& GetAll()
    {
        return Profiler::m_all;
    }

    void Start( const std::string& name, const std::string& category )
    {
#ifdef PROF_ENABLED
        if ( const auto it = Profiler::m_all.find(name); it == Profiler::m_all.end() )
        {
            auto p = std::make_unique<Profiler>(name, category);
            p->Start();

            Profiler::m_all[ name ] = std::move(p);
        } else
            it->second->Start();
#endif
    }

    void End( const std::string& name )
    {
#ifdef PROF_ENABLED
        if ( const auto it = Profiler::m_all.find(name); it != Profiler::m_all.end() )
            it->second->End();
        else
            FATAL_ERROR("Profiler [" + name + "] not found", cmd::ENGINE);
#endif
    }
}
