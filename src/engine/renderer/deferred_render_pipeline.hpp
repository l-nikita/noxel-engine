#pragma once

#include <cstdint>
#include <iostream>
#include <vector>
#include "idepthstencil.hpp"
#include "irenderer.hpp"
#include "irenderfactory.hpp"
#include "irendertexture.hpp"
#include "irenderpipeline.hpp"
#include "ishaderprogram.hpp"

class ModelRenderer;
class RenderCamera;

struct GBuffer
{
    IDepthStencil* DepthStencil;
    IRenderTexture* Color;
    IRenderTexture* Normal; // normal and emission
    IRenderTexture* Position;
    IRenderTexture* Specular; // metalness, roughness, ao, height

    ~GBuffer()
    {
        Release();
    }

    void Release()
    {
        const auto factory = g_Renderer->GetFactory();
        const auto release = [factory]( auto*& res )
        {
            if ( res )
                factory->Release( res );
            res = nullptr;
        };

        release( DepthStencil );
        release( Color );
        release( Normal );
        release( Position );
        release( Specular );
    }
};

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class DeferredRenderPipeline final : public IRenderPipeline
{
public:
    SYSTEM_METADATA( RENDER_PIPELINE, "DeferredRenderPipeline" );

public:
    DeferredRenderPipeline() = default;

    SysInit Initialize() override;
    void Shutdown() override;
    void Render() override;
    void OnWindowResize( int width, int height ) override;

    void DoGeometryPass() const;
    void DoShadowPass() const;
    void DoLightingPass() const;
    void DoSkyboxPass() const;

#ifdef _DEBUG
    void AddDrawCallCount() override
    {
        m_DrawCallCount++;
    }

    std::uint32_t GetDrawCallCount() override
    {
        return m_DrawCallCount;
    }
#endif

public:
#ifdef _DEBUG
    std::uint32_t m_DrawCallCount = 0;
#endif

    std::vector<ModelRenderer*> m_Models;

private:
    void SetupGBuffer( int width, int height );
    void SetupShadows();
    void SetupSkybox();

private:
    IMesh* m_skyboxMesh = nullptr;
    IMaterial* m_skyboxMat = nullptr;

    GBuffer m_GBuffer{};
    IShaderProgram* m_shadowsShader = nullptr;
    IShaderProgram* m_lightingShader = nullptr;

    RenderCamera* m_mainCamera = nullptr;
    IViewport* m_backBufferVP = nullptr;
    IViewport* m_shadowsMapsVP = nullptr;
};
