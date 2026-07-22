#pragma once

#include "common.hpp"

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class IRenderPipeline : public ISystem
{
public:
    virtual void Render() = 0;
    virtual void OnWindowResize( int width, int height ) = 0;

#ifdef _DEBUG
    virtual void AddDrawCallCount() = 0;
    virtual std::uint32_t GetDrawCallCount() = 0;
#endif
};

extern IRenderPipeline* g_RenderPipeline;
