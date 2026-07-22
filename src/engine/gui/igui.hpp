#pragma once

#include "common.hpp"

class Panel;

class IGui : public ISystem
{
public:
    virtual void Render() = 0;
    virtual void Update() = 0;
};

extern IGui* g_Gui;
