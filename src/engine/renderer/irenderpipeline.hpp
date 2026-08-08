#pragma once

#include "common.hpp"


namespace render
{
    //-----------------------------------------------------------------------------

    class Camera;

    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    class IRenderPipeline : public ISystem
    {
    public:
        virtual void Render() = 0;

        virtual Camera* GetCamera() const = 0;
    };
}

extern render::IRenderPipeline* g_RenderPipeline;
