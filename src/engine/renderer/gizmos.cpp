#include <cstdint>
#include <vector>

#include "gizmos.hpp"
#include "imesh.hpp"
#include "irenderer.hpp"
#include "irenderfactory.hpp"
#include "ishaderprogram.hpp"
#include "meshdata.hpp"
#include "vertex.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Gizmos
{
    static IMesh* g_LineMesh;
    static IMesh* g_BoxMesh;
    static IMesh* g_BoxWireMesh;
    static IMesh* g_TriangleMesh;
    static IMaterial* g_Material;

	void Init()
	{
        /*
        auto factory = g_Renderer->GetFactory();
        g_Material = factory->CreateMaterial("gizmos");

        //-------------------------------------------
        std::vector<Vertex> vertices =
        {
            {{0, 0, 0}, {1, 1, 1}, {0,0}, {1,1,1,1}},
            {{1, 0, 0}, {1, 1, 1}, {0,0}, {1,1,1,1}},
        };

        std::vector<std::uint32_t> indices = 
        {
            0, 1
        };

        g_LineMesh = factory->CreateMesh(MeshData{ vertices, indices });

        //-------------------------------------------
        std::vector<Vertex> boxVertices = {
            {{-1,-1, 1}, {1,1,1}, {0,0}, {1,1,1,1}},
            {{ 1,-1, 1}, {1,1,1}, {0,0}, {1,1,1,1}},
            {{ 1, 1, 1}, {1,1,1}, {0,0}, {1,1,1,1}},
            {{-1, 1, 1}, {1,1,1}, {0,0}, {1,1,1,1}},
            {{-1,-1,-1}, {1,1,1}, {0,0}, {1,1,1,1}},
            {{ 1,-1,-1}, {1,1,1}, {0,0}, {1,1,1,1}},
            {{ 1, 1,-1}, {1,1,1}, {0,0}, {1,1,1,1}},
            {{-1, 1,-1}, {1,1,1}, {0,0}, {1,1,1,1}},
        };

        std::vector<std::uint32_t> boxIndices = {
            0,1, 1,2, 2,3, 3,0,  // front face
            4,5, 5,6, 6,7, 7,4,  // back face
            0,4, 1,5, 2,6, 3,7   // connections
        };

        g_BoxWireMesh = factory->CreateMesh(MeshData{ boxVertices, boxIndices });

        std::vector<Vertex> cubeVertices
        {
            // Front
            {{-1, -1,  1}, {1,1,1}, {0,1}, {1,1,1,1}},
            {{ 1, -1,  1}, {1,1,1}, {1,1}, {1,1,1,1}},
            {{ 1,  1,  1}, {1,1,1}, {1,0}, {1,1,1,1}},
            {{-1,  1,  1}, {1,1,1}, {0,0}, {1,1,1,1}},

            // Back
            {{ 1, -1, -1}, {1,1,1}, {0,1}, {1,1,1,1}},
            {{-1, -1, -1}, {1,1,1}, {1,1}, {1,1,1,1}},
            {{-1,  1, -1}, {1,1,1}, {1,0}, {1,1,1,1}},
            {{ 1,  1, -1}, {1,1,1}, {0,0}, {1,1,1,1}},

            // Top
            {{-1,  1,  1}, {1,1,1}, {0,1}, {1,1,1,1}},
            {{ 1,  1,  1}, {1,1,1}, {1,1}, {1,1,1,1}},
            {{ 1,  1, -1}, {1,1,1}, {1,0}, {1,1,1,1}},
            {{-1,  1, -1}, {1,1,1}, {0,0}, {1,1,1,1}},

            // Bottom
            {{-1, -1, -1}, {1,1,1}, {0,1}, {1,1,1,1}},
            {{ 1, -1, -1}, {1,1,1}, {1,1}, {1,1,1,1}},
            {{ 1, -1,  1}, {1,1,1}, {1,0}, {1,1,1,1}},
            {{-1, -1,  1}, {1,1,1}, {0,0}, {1,1,1,1}},

            // Right
            {{ 1, -1,  1}, {1,1,1}, {0,1}, {1,1,1,1}},
            {{ 1, -1, -1}, {1,1,1}, {1,1}, {1,1,1,1}},
            {{ 1,  1, -1}, {1,1,1}, {1,0}, {1,1,1,1}},
            {{ 1,  1,  1}, {1,1,1}, {0,0}, {1,1,1,1}},

            // Left
            {{-1, -1, -1}, {1,1,1}, {0,1}, {1,1,1,1}},
            {{-1, -1,  1}, {1,1,1}, {1,1}, {1,1,1,1}},
            {{-1,  1,  1}, {1,1,1}, {1,0}, {1,1,1,1}},
            {{-1,  1, -1}, {1,1,1}, {0,0}, {1,1,1,1}},
        };

        std::vector<std::uint32_t> cubeIndices
        {
            // Front
            0, 1, 2,  2, 3, 0,
            // Back
            4, 5, 6,  6, 7, 4,
            // Top
            8, 9,10, 10,11, 8,
            // Bottom
            12,13,14, 14,15,12,
            // Right
            16,17,18, 18,19,16,
            // Left
            20,21,22, 22,23,20
        };

        g_BoxMesh = factory->CreateMesh(MeshData{ cubeVertices, cubeIndices });

        //-------------------------------------------
        std::vector<std::uint32_t> triangleIndices =
        {
            0, 1,
            1, 2,
            2, 0
        };

        std::vector<Vertex> verticesTriangle = 
        {
            {{0, 0, 0}, {1,1,1}, {0,0}, {1,1,1,1}},
            {{1, 0, 0}, {1,1,1}, {0,0}, {1,1,1,1}},
            {{0, 1, 0}, {1,1,1}, {0,0}, {1,1,1,1}},
        };

        g_TriangleMesh = factory->CreateMesh(MeshData{ verticesTriangle, triangleIndices });*/
	}

    void Line(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color)
    {
        glm::vec3 dir = end - start;
        float length = glm::length(dir);
        if (length <= 0.0001f)
            return;

        glm::vec3 direction = glm::normalize(dir);
        glm::quat rotation = glm::rotation(glm::vec3(1.0f, 0.0f, 0.0f), direction);

        glm::mat4 model = glm::translate(glm::mat4(1.0f), start) *
            glm::mat4_cast(rotation) *
            glm::scale(glm::mat4(1.0f), glm::vec3(length, 1.0f, 1.0f));

        auto* shader = g_Material->GetShader();
        shader->SetVSVar("model", model);
        shader->SetPSVar("color", color);

        shader->Bind();

        g_Renderer->SetPrimitiveTopology(LINES);
        g_LineMesh->Draw();

        shader->Unbind();
    }

    void Box(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& min, const glm::vec3& max, const glm::vec4& color)
    {
        glm::vec3 scale = (max - min) * 0.5f;
        glm::mat4 model = glm::translate(glm::mat4(1.0f), position)
            * glm::toMat4(rotation)
            * glm::scale(glm::mat4(1.0f), scale);

        auto* shader = g_Material->GetShader();
        shader->SetVSVar("model", model);
        shader->SetPSVar("color", glm::vec4(color.r, color.g, color.b, 1));

        shader->Bind();

        if (color.a < 1)
        {
            g_Renderer->SetPrimitiveTopology(LINES);
            g_BoxWireMesh->Draw();
        }

        if (color.a > 0)
        {
            shader->SetPSVar("color", color);
            shader->Bind();
            g_Renderer->SetPrimitiveTopology(TRIANGLES);
            g_BoxMesh->Draw();
        }

        shader->Unbind();
    }

    void Triangle(const glm::vec3& position, const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3, const glm::vec4& color)
    {
        glm::vec3 edge1 = v2 - v1;
        glm::vec3 edge2 = v3 - v1;
        glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

        glm::mat3 basis(edge1, edge2, normal);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, v1);
        model *= glm::mat4(glm::mat3(basis)); // scale/rotate
        model *= glm::translate(glm::mat4(1.0f), position);

        auto* shader = g_Material->GetShader();
        shader->SetVSVar("model", glm::mat4(1.0f));
        shader->SetPSVar("color", color);

        shader->Bind();

        g_Renderer->SetPrimitiveTopology(LINES);
        g_TriangleMesh->Draw();

        shader->Unbind();
    }

    void Shutdown()
    {
        if (g_LineMesh)
        {
            delete g_LineMesh;
            g_LineMesh = nullptr;
        }

        if (g_BoxMesh)
        {
            delete g_BoxMesh;
            g_BoxMesh = nullptr;
        }

        if (g_BoxWireMesh)
        {
            delete g_BoxWireMesh;
            g_BoxWireMesh = nullptr;
        }

        if (g_Material)
        {
            delete g_Material;
            g_Material = nullptr;
        }
    }
}