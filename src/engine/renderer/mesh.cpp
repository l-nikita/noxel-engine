#include "renderer/mesh.hpp"

namespace render
{
    using namespace Diligent;

    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    Mesh::Mesh( IRenderDevice* device, const MeshData& data )
        : m_indexCount( data.Indices.size() )
    {
        CreateVertexBuffer( device, data );
        CreateIndexBuffer( device, data );

        assert( m_vertexBuffer && m_indexBuffer );
    }

    //-----------------------------------------------------------------------------
    IBuffer* Mesh::GetVertexBuffer() const
    {
        return m_vertexBuffer;
    }

    IBuffer* Mesh::GetIndexBuffer() const
    {
        return m_indexBuffer;
    }

    std::uint32_t Mesh::GetIndexCount() const
    {
        return m_indexCount;
    }

    //-----------------------------------------------------------------------------
    void Mesh::CreateVertexBuffer( IRenderDevice* device, const MeshData& data )
    {
        BufferDesc vbDesc;
        vbDesc.Name = "Mesh Vertex Buffer";
        vbDesc.Size = data.Vertices.size() * sizeof( Vertex );
        vbDesc.Usage = USAGE_IMMUTABLE;
        vbDesc.BindFlags = BIND_VERTEX_BUFFER;

        BufferData vbData;
        vbData.pData = data.Vertices.data();
        vbData.DataSize = data.Vertices.size() * sizeof( Vertex );

        device->CreateBuffer( vbDesc, &vbData, &m_vertexBuffer );
    }

    void Mesh::CreateIndexBuffer( IRenderDevice* device, const MeshData& data )
    {
        BufferDesc ibDesc;
        ibDesc.Name = "Mesh Index Buffer";
        ibDesc.Size = data.Indices.size() * sizeof( std::uint32_t );
        ibDesc.Usage = USAGE_IMMUTABLE;
        ibDesc.BindFlags = BIND_INDEX_BUFFER;

        BufferData ibData;
        ibData.pData = data.Indices.data();
        ibData.DataSize = data.Indices.size() * sizeof( std::uint32_t );

        device->CreateBuffer( ibDesc, &ibData, &m_indexBuffer );
    }
}
