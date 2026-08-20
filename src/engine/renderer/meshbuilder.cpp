#include "renderer/meshbuilder.hpp"

namespace render
{
    //-----------------------------------------------------------------------------

    using namespace Diligent;

    //-----------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------
    MeshBuilder::MeshBuilder( RenderSystem* render )
        : m_render( render )
    {
    }

    MeshData MeshBuilder::CreateCube()
    {
        struct CubeFace
        {
            glm::vec3 Normal;
            glm::vec3 Positions[ 4 ];
        };

        const CubeFace faces[ ] = {
            {{0, 0, 1}, {{-1, -1, 1}, {1, -1, 1}, {1, 1, 1}, {-1, 1, 1}}},
            {{0, 0, -1}, {{1, -1, -1}, {-1, -1, -1}, {-1, 1, -1}, {1, 1, -1}}},
            {{0, 1, 0}, {{-1, 1, 1}, {1, 1, 1}, {1, 1, -1}, {-1, 1, -1}}},
            {{0, -1, 0}, {{-1, -1, -1}, {1, -1, -1}, {1, -1, 1}, {-1, -1, 1}}},
            {{1, 0, 0}, {{1, -1, 1}, {1, -1, -1}, {1, 1, -1}, {1, 1, 1}}},
            {{-1, 0, 0}, {{-1, -1, -1}, {-1, -1, 1}, {-1, 1, 1}, {-1, 1, -1}}}
        };
        const glm::vec2 uvs[ ] = {
            {0, 1},
            {1, 1},
            {1, 0},
            {0, 0}
        };

        MeshData cubeData;
        cubeData.Vertices.reserve( 24 );
        cubeData.Indices.reserve( 36 );

        for ( const auto& [ Normal, Positions ]: faces )
        {
            const auto firstVertex = static_cast<std::uint32_t>(cubeData.Vertices.size());
            for ( std::size_t i = 0; i < std::size( Positions ); ++i )
            {
                cubeData.Vertices.push_back( Vertex{
                    Positions[ i ], Normal, {}, {}, uvs[ i ], glm::vec4( 1.0f )
                } );
            }

            const std::uint32_t faceIndices[ ] = {0, 1, 2, 2, 3, 0};
            for ( const auto index: faceIndices )
                cubeData.Indices.push_back( firstVertex + index );
        }

        CalculateTangents( cubeData.Vertices, cubeData.Indices );

        return cubeData;
    }

    void MeshBuilder::CalculateTangents( std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices )
    {
        for ( auto& v: vertices )
        {
            v.Tangent = glm::vec3( 0.0f );
            v.Bitangent = glm::vec3( 0.0f );
        }

        for ( size_t i = 0; i < indices.size(); i += 3 )
        {
            Vertex& v0 = vertices[ indices[ i + 0 ] ];
            Vertex& v1 = vertices[ indices[ i + 1 ] ];
            Vertex& v2 = vertices[ indices[ i + 2 ] ];

            glm::vec3 edge1 = v1.Position - v0.Position;
            glm::vec3 edge2 = v2.Position - v0.Position;

            glm::vec2 deltaUV1 = v1.UV - v0.UV;
            glm::vec2 deltaUV2 = v2.UV - v0.UV;

            float f = 1.0f / ( deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y );

            glm::vec3 tangent;
            glm::vec3 bitangent;

            tangent.x = f * ( deltaUV2.y * edge1.x - deltaUV1.y * edge2.x );
            tangent.y = f * ( deltaUV2.y * edge1.y - deltaUV1.y * edge2.y );
            tangent.z = f * ( deltaUV2.y * edge1.z - deltaUV1.y * edge2.z );

            bitangent.x = f * ( -deltaUV2.x * edge1.x + deltaUV1.x * edge2.x );
            bitangent.y = f * ( -deltaUV2.x * edge1.y + deltaUV1.x * edge2.y );
            bitangent.z = f * ( -deltaUV2.x * edge1.z + deltaUV1.x * edge2.z );

            v0.Tangent += tangent;
            v1.Tangent += tangent;
            v2.Tangent += tangent;
            v0.Bitangent += bitangent;
            v1.Bitangent += bitangent;
            v2.Bitangent += bitangent;
        }

        for ( auto& v: vertices )
        {
            v.Tangent = glm::normalize( v.Tangent - v.Normal * glm::dot( v.Normal, v.Tangent ) );
            v.Bitangent = glm::normalize( glm::cross( v.Normal, v.Tangent ) );
        }
    }
}
