#include <Graphics/GraphicsEngine/interface/RenderDevice.h>

#include "dbg/debug.hpp"
#include "assetsystem/iassetsystem.hpp"
#include "assetsystem/shaderdata.hpp"
#include "assetsystem/texturedata.hpp"
#include "renderer/factory.hpp"

#include "renderer/rendersystem.hpp"

namespace render
{
    //-----------------------------------------------------------------------------

    using namespace Diligent;

    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    ShaderPsVs Factory::CreateShader( const assets::Id& id )
    {
        auto data = g_Assets->LoadData<assets::ShaderData>( id );
        auto device = g_Render->GetDevice();

        auto vsSrc = g_FileSystem->ReadText( g_FileSystem->Absolute( data->VertexShaderPath ) );
        if ( vsSrc.empty() )
            FATAL_ERROR( "Vertex shader not found", cmd::RENDERING );

        auto psSrc = g_FileSystem->ReadText( g_FileSystem->Absolute( data->PixelShaderPath ) );
        if ( psSrc.empty() )
            FATAL_ERROR( "Pixel shader not found", cmd::RENDERING );

        ShaderCreateInfo info;
        info.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;
        info.Desc.UseCombinedTextureSamplers = false;

        auto vsName = data->Name;
        vsName += "_vs";

        RefCntAutoPtr<IShader> vs;
        info.Desc.ShaderType = SHADER_TYPE_VERTEX;
        info.EntryPoint = "main";
        info.Desc.Name = vsName.c_str();
        info.Source = vsSrc.c_str();
        device->CreateShader( info, &vs );

        auto psName = data->Name;
        psName += "_ps";

        RefCntAutoPtr<IShader> ps;
        info.Desc.ShaderType = SHADER_TYPE_PIXEL;
        info.EntryPoint = "main";
        info.Desc.Name = psName.c_str();
        info.Source = psSrc.c_str();
        device->CreateShader( info, &ps );

        return {vs, ps};
    }

    RefCntAutoPtr<ITexture> Factory::CreateTexture( const assets::Id& id )
    {
        auto asset = g_Assets->GetAsset( id );
        auto data = g_Assets->LoadData<assets::TextureData>( asset );

        TextureDesc texDesc;
        texDesc.Name = asset->Name.c_str();
        texDesc.Type = RESOURCE_DIM_TEX_2D;
        texDesc.Width = data->Width;
        texDesc.Height = data->Height;
        texDesc.Format = TEX_FORMAT_RGBA8_UNORM;
        texDesc.MipLevels = 1;
        texDesc.BindFlags = BIND_SHADER_RESOURCE;
        texDesc.Usage = USAGE_IMMUTABLE;

        TextureSubResData subresource;
        subresource.pData = data->Image;
        subresource.Stride =
                data->Width * 4u;

        TextureData initData;
        initData.pSubResources = &subresource;
        initData.NumSubresources = 1;

        RefCntAutoPtr<ITexture> texture;

        auto device = g_Render->GetDevice();
        device->CreateTexture( texDesc, &initData, &texture );

        if ( !texture )
            FATAL_ERROR( "Failed to create texture: " + asset->Name, cmd::RENDERING );

        return texture;
    }
}
