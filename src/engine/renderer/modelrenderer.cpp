#include "renderer/modelrenderer.hpp"

#include <cassert>

#include "renderer/material.hpp"
#include "renderer/mesh.hpp"

namespace render
{
    //-------------------------------------------------------------------------

    using namespace Diligent;

    //-------------------------------------------------------------------------
    //
    //-------------------------------------------------------------------------
    ModelRenderer::ModelRenderer( Mesh* mesh, Material* material )
    {
        AddPart( mesh, material );
    }

    ModelRenderer::ModelRenderer( const std::vector<Mesh*>& meshes, const std::vector<Material*>& materials )
    {
        m_parts.reserve( meshes.size() );

        for ( std::size_t i = 0; i < meshes.size(); ++i )
        {
            Material* material = i < materials.size() ? materials[ i ] : nullptr;
            AddPart( meshes[ i ], material );
        }
    }

    //-------------------------------------------------------------------------
    void ModelRenderer::AddPart( Mesh* mesh, Material* material )
    {
        assert( mesh );
        m_parts.push_back( ModelPart{mesh, material} );
    }

    void ModelRenderer::SetMesh( Mesh* mesh, const std::size_t index )
    {
        assert( mesh );
        assert( index < m_parts.size() );

        if ( index < m_parts.size() )
            m_parts[ index ].MeshResource = mesh;
    }

    void ModelRenderer::SetMaterial( Material* material, const std::size_t index )
    {
        assert( index < m_parts.size() );

        if ( index < m_parts.size() )
            m_parts[ index ].MaterialResource = material;
    }

    //-------------------------------------------------------------------------
    Mesh* ModelRenderer::GetMesh( const std::size_t index ) const
    {
        const auto* part = GetPart( index );
        return part ? part->MeshResource : nullptr;
    }

    Material* ModelRenderer::GetMaterial( const std::size_t index ) const
    {
        const auto* part = GetPart( index );
        return part ? part->MaterialResource : nullptr;
    }

    const ModelPart* ModelRenderer::GetPart( const std::size_t index ) const
    {
        return index < m_parts.size() ? &m_parts[ index ] : nullptr;
    }

    const std::vector<ModelPart>& ModelRenderer::GetParts() const
    {
        return m_parts;
    }

    std::size_t ModelRenderer::GetPartCount() const
    {
        return m_parts.size();
    }

    //-------------------------------------------------------------------------
    void ModelRenderer::DrawPart( IDeviceContext* context, const std::size_t index ) const
    {
        assert( context );

        auto mesh = GetPart( index )->MeshResource;
        IBuffer* vertexBuffers[ ] = {mesh->GetVertexBuffer()};
        constexpr std::uint64_t offsets[ ] = {0};

        context->SetVertexBuffers( 0, std::size( vertexBuffers ), vertexBuffers, offsets,
                                   RESOURCE_STATE_TRANSITION_MODE_TRANSITION, SET_VERTEX_BUFFERS_FLAG_RESET );

        context->SetIndexBuffer( mesh->GetIndexBuffer(), 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION );

        DrawIndexedAttribs drawAttribs;
        drawAttribs.NumIndices = mesh->GetIndexCount();
        drawAttribs.IndexType = VT_UINT32;

#ifdef _DEBUG
        drawAttribs.Flags = DRAW_FLAG_VERIFY_ALL;
#endif

        context->DrawIndexed( drawAttribs );
    }
}
