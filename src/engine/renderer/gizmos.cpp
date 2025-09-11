#include <cstdint>
#include <vector>

#include "gizmos.hpp"
#include "imesh.hpp"
#include "irenderer.hpp"
#include "irenderfactory.hpp"
#include "render_camera.hpp"
#include "iengine.hpp"
#include "meshdata.hpp"
#include "vertex.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Gizmos
{
    static IMesh* g_LineMesh;
    static IMaterial* g_Material;

	void Init()
	{
        std::vector<Vertex> vertices =
        {
            {{0, 0, 0}, {1,1,1,1}, {0,0}},
            {{1, 0, 0}, {1,1,1,1}, {0,0}},
        };

        std::vector<uint32_t> indices = 
        {
            0, 1
        };

        auto* factory = g_Renderer->GetFactory();
        g_LineMesh = factory->CreateMesh(MeshData{ vertices, indices });
        g_Material = factory->CreateMaterial("gizmos");
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
        g_Renderer->SetPrimitiveTopology(LINES);
        shader->Bind();
        shader->SetVSParameter("model", model);
        shader->SetPSParameter("color", color);

        g_LineMesh->Draw();

        shader->Unbind();
    }

    void Shutdown()
    {
        delete g_LineMesh;
    }
}