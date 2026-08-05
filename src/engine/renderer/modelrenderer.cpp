#include "modelrenderer.hpp"
#include "irenderer.hpp"
#include "irenderfactory.hpp"
#include "ishaderprogram.hpp"
#include "dbg/debug.hpp"

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
ModelRenderer::ModelRenderer( const std::vector<IMesh*>& meshes, const std::vector<IMaterial*>& materials )
{
    m_meshes = meshes;
    m_materials.reserve( meshes.size() );

    for ( size_t i = 0; i < m_meshes.size(); ++i )
    {
        auto mat = ( i < materials.size() && materials[ i ] )
                       ? materials[ i ]
                       : g_ErrorMaterial;

        m_materials.push_back( mat );
    }
}

ModelRenderer::ModelRenderer( IMesh* mesh, IMaterial* material )
{
    m_meshes.push_back( mesh );

    if ( !material )
        material = g_ErrorMaterial;

    m_materials.push_back( material );
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void ModelRenderer::SetMaterial( IMaterial* material, size_t index )
{
    assert( material );
    assert( index < m_materials.size() );

    m_materials[ index ] = material;
}

//-----------------------------------------------------------------------------
IMaterial* ModelRenderer::GetMaterial() const
{
    return !m_materials.empty() ? m_materials[ 0 ] : nullptr;
}

IMaterial* ModelRenderer::GetMaterial( size_t index ) const
{
    return ( index < m_materials.size() ) ? m_materials[ index ] : nullptr;
}

std::vector<IMaterial*>& ModelRenderer::GetMaterials()
{
    return m_materials;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
IMesh* ModelRenderer::GetMesh() const
{
    return !m_meshes.empty() ? m_meshes[ 0 ] : nullptr;
}

IMesh* ModelRenderer::GetMesh( size_t index ) const
{
    return ( index < m_meshes.size() ) ? m_meshes[ index ] : nullptr;
}

std::vector<IMesh*>& ModelRenderer::GetMeshes()
{
    return m_meshes;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void ModelRenderer::Draw() const
{
    for ( size_t i = 0; i < m_meshes.size(); ++i )
    {
        const auto mat = GetMaterial( i );
        if ( !mat )
        {
            CMD_ERROR( "Material is invalid!", CMD::RENDERING );
            continue;
        }

        mat->GetShader()->SetVSVar( "model", m_Transform.GetTransformMatrix() );
        mat->Bind();
        g_Renderer->SetPrimitiveTopology( TRIANGLES );
        m_meshes[ i ]->Draw();
        mat->Unbind();
    }
}
