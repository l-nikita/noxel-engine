#include <vector>
#include <string>

#include "profiler.hpp"

std::vector<Profiler*> Profiler::All;

Profiler::Profiler(const std::string& name)
{
    _name = name;
    All.push_back(this);
    _start = std::chrono::high_resolution_clock::now();
}

void Profiler::Start() 
{
    _start = std::chrono::high_resolution_clock::now();
}

void Profiler::End() 
{
    _end = std::chrono::high_resolution_clock::now();
    _lastFrameTime = std::chrono::duration<float, std::milli>(_end - _start).count();
}