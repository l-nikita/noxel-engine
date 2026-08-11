#pragma once

#include <Common/interface/RefCntAutoPtr.hpp>
#include <Graphics/GraphicsEngine/interface/DeviceContext.h>
#include <Graphics/GraphicsEngine/interface/RenderDevice.h>
#include <Graphics/GraphicsEngine/interface/SwapChain.h>

#include "common.hpp"
#include "engine/window.hpp"

namespace render
{
    //-----------------------------------------------------------------------------

    constexpr float CLEAR_COLOR[ ] = {0.1f, 0.2f, 0.3f, 1.0f};

    //-----------------------------------------------------------------------------

    using IRenderDevice = Diligent::IRenderDevice;
    using IDeviceContext = Diligent::IDeviceContext;
    using ISwapChain = Diligent::ISwapChain;
    using IPipelineState = Diligent::IPipelineState;
    using IShader = Diligent::IShader;
    using ITextureView = Diligent::ITextureView;
    using ITexture = Diligent::ITexture;
    using IBuffer = Diligent::IBuffer;
    using IShaderResourceBinding = Diligent::IShaderResourceBinding;
    using IEngineFactory = Diligent::IEngineFactory;
    using IShaderSourceInputStreamFactory = Diligent::IShaderSourceInputStreamFactory;

    //-----------------------------------------------------------------------------

    class Factory;

    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    struct DefaultTextures
    {
        Diligent::RefCntAutoPtr<ITexture> Black;
        Diligent::RefCntAutoPtr<ITexture> White;
        Diligent::RefCntAutoPtr<ITexture> NeutralNormal;
    };

    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    class RenderSystem final : public ISystem
    {
    public:
        SYSTEM_METADATA( RENDER, "RenderSystem" );

    public:
        explicit RenderSystem( Window* window );

        SysInit Initialize() override;
        void Shutdown() override;

        Window* GetWindow() const;
        void Present();

        IRenderDevice* GetDevice() const;
        IDeviceContext* GetContext() const;
        ISwapChain* GetSwapChain() const;
        IEngineFactory* GetEngineFactory() const;

        Factory* GetFactory() const;
        const DefaultTextures& GetDefaultTextures();

    private:
        SysInit CreateDeviceAndSwapChain();

        void CreateDefaultTextures();

    private:
        Window* m_window = nullptr;
        std::unique_ptr<Factory> m_factory;

        DefaultTextures m_defTextures = {};

    private:
        Diligent::RefCntAutoPtr<IRenderDevice> m_device;
        Diligent::RefCntAutoPtr<IDeviceContext> m_context;
        Diligent::RefCntAutoPtr<ISwapChain> m_swapChain;
        Diligent::RefCntAutoPtr<IPipelineState> m_pso;
        Diligent::RefCntAutoPtr<IEngineFactory> m_engineFactory;
    };
}

extern render::RenderSystem* g_Render;
