#pragma once

#include <Common/interface/RefCntAutoPtr.hpp>

#include "renderer/meshdata.hpp"
#include "renderer/rendersystem.hpp"

namespace render
{
    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    class Mesh
    {
    public:
        Mesh( IRenderDevice* device, const MeshData& data );

        IBuffer* GetVertexBuffer() const;
        IBuffer* GetIndexBuffer() const;
        std::uint32_t GetIndexCount() const;

    private:
        Diligent::RefCntAutoPtr<IBuffer> m_vertexBuffer;
        Diligent::RefCntAutoPtr<IBuffer> m_indexBuffer;
        std::uint32_t m_indexCount;

    private:
        void CreateVertexBuffer( IRenderDevice* device, const MeshData& data );
        void CreateIndexBuffer( IRenderDevice* device, const MeshData& data );
    };
}
