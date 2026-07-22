#include "dx11buffer.hpp"
#include "dx11mesh.hpp"
#include "renderer/irenderpipeline.hpp"
#include <stdexcept>
#include <cstdint>
#include <iostream>
#include <d3d11.h>

DX11Mesh::DX11Mesh( ID3D11Device* device, ID3D11DeviceContext* context, const MeshData& data )
    : m_vertexCount( std::size( data.Vertices ) ), m_indexCount( std::size( data.Indices ) ),
      m_vertexStride( sizeof( Vertex ) ),
      m_context( context )
{
    m_vertexBuffer = new DX11Buffer(
        device,
        context,
        GPUBufferType::VERTEX,
        m_vertexStride * m_vertexCount,
        data.Vertices.data(),
        m_vertexStride );

    m_indexBuffer = new DX11Buffer(
        device,
        context,
        GPUBufferType::INDEX,
        sizeof( std::uint32_t ) * m_indexCount,
        data.Indices.data() );
}

void DX11Mesh::Bind()
{
    if ( m_vertexBuffer ) m_vertexBuffer->Bind( 0 );
    if ( m_indexBuffer ) m_indexBuffer->Bind( 0 );
}

void DX11Mesh::Unbind()
{
    if ( m_vertexBuffer ) m_vertexBuffer->Unbind( 0 );
    if ( m_indexBuffer ) m_indexBuffer->Unbind( 0 );
}

void DX11Mesh::Draw()
{
    Bind();
    m_context->DrawIndexed( m_indexCount, 0, 0 );
#ifdef _DEBUG
    g_RenderPipeline->AddDrawCallCount();
#endif
    Unbind();
}

void DX11Mesh::Release()
{
    if ( IsReleased() )
        return;

    m_isReleased = true;

    if ( m_vertexBuffer )
        delete m_vertexBuffer, m_vertexBuffer = nullptr;

    if ( m_indexBuffer )
        delete m_indexBuffer, m_indexBuffer = nullptr;
}
