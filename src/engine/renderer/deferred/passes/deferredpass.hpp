#pragma once

#include "renderer/rendersystem.hpp"

namespace render::deferred
{
    class DeferredRenderPipeline;
}

namespace render::deferred::passes
{
    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    struct DeferredPassContext
    {
        RenderSystem* Render;
        DeferredRenderPipeline* Pipeline;
    };

    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    class DeferredPass
    {
    public:
        explicit DeferredPass( const DeferredPassContext& ctx )
            : m_render( ctx.Render ), m_pipeline( ctx.Pipeline )
        {
        }

    protected:
        RenderSystem* m_render;
        DeferredRenderPipeline* m_pipeline;
    };
}
