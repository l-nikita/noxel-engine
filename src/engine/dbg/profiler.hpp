#pragma once

#include <iostream>
#include <chrono>
#include <vector>
#include <string>

#include "common.hpp"

class ENGINE_API Profiler
{
public:
    static std::vector<Profiler*> All;
    Profiler(const std::string& name);

private:
    std::string _name;
    std::chrono::high_resolution_clock::time_point _start;
    std::chrono::high_resolution_clock::time_point _end;
    float _lastFrameTime = 0.0f;

public:
    void Start();
    void End();

    float GetTime() const { return _lastFrameTime; }
    std::string GetName() const { return _name; }
};