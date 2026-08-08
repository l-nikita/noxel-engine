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

        Diligent::RefCntAutoPtr<IRenderDevice> GetDevice();
        Diligent::RefCntAutoPtr<IDeviceContext> GetContext();
        Diligent::RefCntAutoPtr<ISwapChain> GetSwapChain();

        const DefaultTextures& GetDefaultTextures();

    private:
        SysInit CreateDeviceAndSwapChain();

        void CreateDefaultTextures();

    private:
        Window* m_window = nullptr;

        DefaultTextures m_defTextures = {};

        Diligent::RefCntAutoPtr<IRenderDevice> m_device;
        Diligent::RefCntAutoPtr<IDeviceContext> m_context;
        Diligent::RefCntAutoPtr<ISwapChain> m_swapChain;
        Diligent::RefCntAutoPtr<IPipelineState> m_pso;
    };
}

extern render::RenderSystem* g_Render;
