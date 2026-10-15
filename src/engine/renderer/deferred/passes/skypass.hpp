#pragma once

#include "assetsystem/shaderdata.hpp"
#include "renderer/camera.hpp"
#include "renderer/mesh.hpp"
#include "renderer/modelrenderer.hpp"
#include "renderer/deferred/passes/deferredpass.hpp"

namespace render::deferred::passes
{
    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    struct SkyPassContext
    {
        render::Camera* Camera;
        ITextureView* SkyTextureView;
    };

    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    class SkyPass final : public DeferredPass
    {
    public:
        explicit SkyPass( const DeferredPassContext& ctx );

        void Setup();
        void Render( const SkyPassContext& ctx );

    private:
        void SetupPSO();
        void SetupConstantBuffers();
        void SetupSRB();
        void SetupCubemap();

        void UpdateTransform( const Camera* camera );

    private:
        std::unique_ptr<assets::ShaderData> m_shaderData;

        Diligent::RefCntAutoPtr<IPipelineState> m_PSO;
        Diligent::RefCntAutoPtr<IShaderResourceBinding> m_SRB;

        Diligent::RefCntAutoPtr<IBuffer> m_transformCB;

        std::unique_ptr<Mesh> m_cubeMesh;
        std::unique_ptr<ModelRenderer> m_cubeModel;
    };
}
