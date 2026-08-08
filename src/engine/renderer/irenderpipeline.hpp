#pragma once

#include "common.hpp"

namespace render
{
    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    class IRenderPipeline : public ISystem
    {
    public:
        virtual void Render() = 0;
    };
}

extern render::IRenderPipeline* g_RenderPipeline;
