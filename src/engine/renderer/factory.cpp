#include <cassert>
#include <ranges>
#include <Graphics/GraphicsEngine/interface/RenderDevice.h>

#include "renderer/factory.hpp"

#include "assetsystem/iassetsystem.hpp"
#include "assetsystem/materialdata.hpp"
#include "assetsystem/shaderdata.hpp"
#include "assetsystem/texturedata.hpp"
#include "dbg/debug.hpp"
#include "renderer/rendersystem.hpp"

namespace render
{
    //-----------------------------------------------------------------------------

    using namespace Diligent;

    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    Factory::Factory( RenderSystem* render )
        : m_render( render )
    {
        auto shaderPath = g_FileSystem->Absolute( "assets/shaders" ).string();
        auto cStr = shaderPath.c_str();

        render->GetEngineFactory()->CreateDefaultShaderSourceStreamFactory(
            cStr,
            &m_shaderSourceFactory
        );
    }

    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    Shader Factory::CreateShader( const assets::Id& id )
    {
        auto data = g_Assets->LoadData<assets::ShaderData>( id );
        auto device = m_render->GetDevice();

        auto vsPath = g_FileSystem->Absolute( data->VertexShaderPath );
        auto psPath = g_FileSystem->Absolute( data->PixelShaderPath );

        auto vsSrc = g_FileSystem->ReadText( vsPath );
        if ( vsSrc.empty() )
            FATAL_ERROR( "Vertex shader not found", cmd::RENDERING );

        auto psSrc = g_FileSystem->ReadText( psPath );
        if ( psSrc.empty() )
            FATAL_ERROR( "Pixel shader not found", cmd::RENDERING );

        ShaderCreateInfo info;
        info.Desc.UseCombinedTextureSamplers = false;
        info.pShaderSourceStreamFactory = m_shaderSourceFactory;

        if ( data->SourceLanguage == "HLSL" )
            info.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;
        else if ( data->SourceLanguage == "GLSL" )
            info.SourceLanguage = SHADER_SOURCE_LANGUAGE_GLSL;

        auto vsName = data->Name;
        vsName += "_vs";

        Shader shader = {};

        info.Desc.ShaderType = SHADER_TYPE_VERTEX;
        info.EntryPoint = "main";
        info.Desc.Name = vsName.c_str();
        info.Source = vsSrc.c_str();
        device->CreateShader( info, &shader.VertexShader );

        auto psName = data->Name;
        psName += "_ps";

        info.Desc.ShaderType = SHADER_TYPE_PIXEL;
        info.EntryPoint = "main";
        info.Desc.Name = psName.c_str();
        info.Source = psSrc.c_str();
        device->CreateShader( info, &shader.PixelShader );

        return shader;
    }

    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    void Factory::CreatePipelineState( GraphicsPipelineStateCreateInfo& info, const assets::ShaderData& data,
                                       IPipelineState** state )
    {
        auto device = m_render->GetDevice();

        //-----------------------------------------------------------------------------
        std::vector<ShaderResourceVariableDesc> vars;

        for ( auto& texParam: data.Textures | std::views::values )
        {
            auto type = texParam.VarType;
            auto srvType = SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE;

            if ( type == "STATIC" )
                srvType = SHADER_RESOURCE_VARIABLE_TYPE_STATIC;
            else if ( type == "DYNAMIC" )
                srvType = SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC;

            vars.emplace_back( SHADER_TYPE_PIXEL, texParam.VarBind.c_str(), srvType );
        }

        info.PSODesc.ResourceLayout.Variables = vars.data();
        info.PSODesc.ResourceLayout.NumVariables = static_cast<std::uint32_t>(vars.size());

        //-----------------------------------------------------------------------------
        std::vector<ImmutableSamplerDesc> immutableSamplers;

        for ( auto& [ samBindName, sampParam ]: data.Samplers )
        {
            SamplerDesc sampler;

            auto minFilter = sampParam.MinFilter;
            auto magFilter = sampParam.MagFilter;
            auto mipFilter = sampParam.MipFilter;

            auto addressU = sampParam.AddressU;
            auto addressV = sampParam.AddressV;
            auto addressW = sampParam.AddressW;

            if ( minFilter == "LINEAR" )
                sampler.MinFilter = FILTER_TYPE_LINEAR;
            else if ( minFilter == "POINT" )
                sampler.MinFilter = FILTER_TYPE_POINT;

            if ( magFilter == "LINEAR" )
                sampler.MagFilter = FILTER_TYPE_LINEAR;
            else if ( magFilter == "POINT" )
                sampler.MagFilter = FILTER_TYPE_POINT;

            if ( mipFilter == "LINEAR" )
                sampler.MipFilter = FILTER_TYPE_LINEAR;
            else if ( mipFilter == "POINT" )
                sampler.MipFilter = FILTER_TYPE_POINT;

            if ( addressU == "WARP" )
                sampler.AddressU = TEXTURE_ADDRESS_WRAP;
            else if ( addressU == "CLAMP" )
                sampler.AddressU = TEXTURE_ADDRESS_CLAMP;

            if ( addressV == "WARP" )
                sampler.AddressV = TEXTURE_ADDRESS_WRAP;
            else if ( addressV == "CLAMP" )
                sampler.AddressV = TEXTURE_ADDRESS_CLAMP;

            if ( addressW == "WARP" )
                sampler.AddressW = TEXTURE_ADDRESS_WRAP;
            else if ( addressW == "CLAMP" )
                sampler.AddressW = TEXTURE_ADDRESS_CLAMP;

            immutableSamplers.emplace_back( SHADER_TYPE_PIXEL, samBindName.c_str(), sampler );
        }

        if ( immutableSamplers.empty() )
            FATAL_ERROR( "No immutable samplers for '" + data.Name + "' found", cmd::RENDERING );

        info.PSODesc.ResourceLayout.ImmutableSamplers = immutableSamplers.data();
        info.PSODesc.ResourceLayout.NumImmutableSamplers = static_cast<std::uint32_t>(immutableSamplers.size());

        device->CreateGraphicsPipelineState( info, state );
    }

    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    RefCntAutoPtr<ITexture> Factory::CreateTexture( const assets::Id& id )
    {
        auto asset = g_Assets->GetAsset( id );
        auto data = g_Assets->LoadData<assets::TextureData>( asset );
        assert( data );

        TextureDesc desc;
        desc.Name = asset->Name.c_str();
        desc.Type = RESOURCE_DIM_TEX_2D;
        desc.Width = data->Width;
        desc.Height = data->Height;
        desc.Format = TEX_FORMAT_RGBA8_UNORM;
        desc.MipLevels = 1;
        desc.BindFlags = BIND_SHADER_RESOURCE;
        desc.Usage = USAGE_IMMUTABLE;

        TextureSubResData subresource;
        subresource.pData = data->Image;
        subresource.Stride = data->Width * 4u;

        TextureData initData;
        initData.pSubResources = &subresource;
        initData.NumSubresources = 1;

        RefCntAutoPtr<ITexture> texture;

        auto device = m_render->GetDevice();
        device->CreateTexture( desc, &initData, &texture );

        if ( !texture )
            FATAL_ERROR( "Failed to create texture: " + asset->Name, cmd::RENDERING );

        return texture;
    }

    //-----------------------------------------------------------------------------
    RefCntAutoPtr<ITexture> Factory::CreateTextureCube( const TextureCubeData& cubeData )
    {
        std::array<std::unique_ptr<assets::TextureData>, 6> data;

        for ( std::size_t i = 0; i < 6; i++ )
        {
            auto asset = g_Assets->GetAsset( cubeData.Faces[ i ] );
            data[ i ] = g_Assets->LoadData<assets::TextureData>( asset );

            assert( data[i] );
        }

        const auto width = data[ 0 ]->Width;
        const auto height = data[ 0 ]->Height;

        for ( std::size_t i = 1; i < 6; i++ )
        {
            assert( data[i]->Width == width );
            assert( data[i]->Height == height );
        }

        TextureDesc desc;
        desc.Name = "TextureCube";
        desc.Type = RESOURCE_DIM_TEX_CUBE;
        desc.Width = width;
        desc.Height = height;
        desc.ArraySize = 6;
        desc.Format = TEX_FORMAT_RGBA8_UNORM;
        desc.MipLevels = 1;
        desc.BindFlags = BIND_SHADER_RESOURCE;
        desc.Usage = USAGE_IMMUTABLE;

        std::array<TextureSubResData, 6> subresources;
        for ( std::size_t i = 0; i < 6; i++ )
        {
            subresources[ i ].pData = data[ i ]->Image;
            subresources[ i ].Stride = width * data[ i ]->Channels;
        }

        TextureData initData;
        initData.pSubResources = subresources.data();
        initData.NumSubresources = 6;

        RefCntAutoPtr<ITexture> texture;

        auto device = m_render->GetDevice();
        device->CreateTexture( desc, &initData, &texture );

        if ( !texture )
            FATAL_ERROR( "Failed to create texture cube", cmd::RENDERING );

        return texture;
    }

    //-----------------------------------------------------------------------------
    RefCntAutoPtr<ITexture> Factory::CreateTextureCube( const assets::Id& matId )
    {
        auto matData = g_Assets->LoadData<assets::MaterialData>( matId );
        assert( matData );
        assert( matData->IsCubemap );

        TextureCubeData cubeData = {};

        int i = 0;
        for ( auto faceName: CUBEMAP_FACES_ORDER )
        {
            for ( auto [ texName, texParam ]: matData->TexturesParameters )
            {
                if ( texName == faceName )
                    if ( auto val = std::get_if<assets::Id>( &texParam ) )
                        cubeData.Faces[ i ] = *val, i++;
            }
        }

        return CreateTextureCube( cubeData );
    }

    //-----------------------------------------------------------------------------
    RefCntAutoPtr<ITexture> Factory::CreateSolidTexture( const std::array<std::uint8_t, 4>& color,
                                                         const std::string& name )
    {
        auto device = m_render->GetDevice();

        TextureDesc desc;
        desc.Name = name.c_str();
        desc.Type = RESOURCE_DIM_TEX_2D;
        desc.Width = 1;
        desc.Height = 1;
        desc.Format = TEX_FORMAT_RGBA8_UNORM;
        desc.MipLevels = 1;
        desc.BindFlags = BIND_SHADER_RESOURCE;
        desc.Usage = USAGE_IMMUTABLE;

        TextureSubResData subresource;
        subresource.pData = color.data();
        subresource.Stride = 4;

        TextureData initData;
        initData.pSubResources = &subresource;
        initData.NumSubresources = 1;

        RefCntAutoPtr<ITexture> texture;
        device->CreateTexture( desc, &initData, &texture );

        if ( !texture )
            FATAL_ERROR( std::string{"Failed to create texture: "} + name, cmd::RENDERING );

        return texture;
    }

    RefCntAutoPtr<ITexture> Factory::CreateSolidTextureCube( const std::array<std::uint8_t, 4>& color,
                                                             const std::string& name )
    {
        TextureDesc desc;
        desc.Name = "TextureCube";
        desc.Type = RESOURCE_DIM_TEX_CUBE;
        desc.Width = 1;
        desc.Height = 1;
        desc.ArraySize = 6;
        desc.Format = TEX_FORMAT_RGBA8_UNORM;
        desc.MipLevels = 1;
        desc.BindFlags = BIND_SHADER_RESOURCE;
        desc.Usage = USAGE_IMMUTABLE;

        std::array<TextureSubResData, 6> subresources;
        for ( std::size_t i = 0; i < 6; i++ )
        {
            subresources[ i ].pData = color.data();
            subresources[ i ].Stride = 1 * 4u;
        }

        TextureData initData;
        initData.pSubResources = subresources.data();
        initData.NumSubresources = 6;

        RefCntAutoPtr<ITexture> texture;

        auto device = m_render->GetDevice();
        device->CreateTexture( desc, &initData, &texture );

        if ( !texture )
            FATAL_ERROR( "Failed to create texture cube", cmd::RENDERING );

        return texture;
    }
}
