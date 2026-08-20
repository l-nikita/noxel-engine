#pragma once


#include "renderer/meshdata.hpp"
#include "renderer/rendersystem.hpp"

namespace render
{
    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    class MeshBuilder final
    {
    public:
        explicit MeshBuilder( RenderSystem* render );

        // Default meshes
        MeshData CreateCube();

        void CalculateTangents( std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices );

    private:
        RenderSystem* m_render;
    };
}
