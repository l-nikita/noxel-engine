#include <random>

#include "entities.hpp"
#include "cmd/cmd.hpp"
#include "renderer/model.hpp"
#include "renderer/gizmos.hpp"
#include "physics/physics.hpp"
#include "server/player.hpp"
#include "input/input.hpp"

static std::vector<entt::entity> Players;
static std::unique_ptr<Model> TestModel;

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool Entities::Init()
{
    m_isRunning = true;

    //for (int i = 0; i < 25; i++)
    //{
    //    Entity entity = Entities::CreateEntity();
    //    entity.Add<Transform>();
    //    entity.Add<EModel>(model);

    //    entity.Get<Transform>().Position = glm::vec3(i * 1.0f, 0.0f, 0.0f);
    //}

    std::vector<float> vertices =
    {
        //right (z)
        0.5f,  0.5f, 0.5f,
        0.5f, -0.5f, 0.5f,
       -0.5f, -0.5f, 0.5f,
       -0.5f,  0.5f, 0.5f,

       //left (-z)
       0.5f,  0.5f, -0.5f,
       0.5f, -0.5f, -0.5f,
      -0.5f, -0.5f, -0.5f,
      -0.5f,  0.5f, -0.5f,

      //forward (x)
      0.5f,  0.5f, -0.5f,
      0.5f,  0.5f,  0.5f,
      0.5f, -0.5f,  0.5f,
      0.5f, -0.5f, -0.5f,

      //backward (-x)
     -0.5f,  0.5f,  0.5f,
     -0.5f,  0.5f, -0.5f,
     -0.5f, -0.5f, -0.5f,
     -0.5f, -0.5f,  0.5f,

     //top (y)
     0.5f, 0.5f, -0.5f,
    -0.5f, 0.5f, -0.5f,
    -0.5f, 0.5f, 0.5f,
     0.5f, 0.5f, 0.5f,

     //bottom (y)
     0.5f, -0.5f, 0.5f,
    -0.5f, -0.5f, 0.5f,
    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
    };

    std::vector<int> indices =
    {
        0, 1, 3,
        1, 2, 3,

        7, 6, 4,
        6, 5, 4,

        11, 10, 8,
        10, 9, 8,

        15, 14, 12,
        14, 13, 12,

        19, 18, 16,
        18, 17, 16,

        23, 22, 20,
        22, 21, 20,
    };

    std::vector<float> uv =
    {
        //right (z)
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,

        //left (-z)
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,

        //forward (x)
        1.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,

        //backward (-x)
        1.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,

        //top (y)
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,

        //bottom (-y)
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
    };

    MeshData data(vertices, indices, uv, GL_TRIANGLES);
    auto mesh = std::make_shared<Mesh>(data);
    auto mat = Material::Get("dirt");
    TestModel = std::make_unique<Model>(mesh, mat);

    auto player = CreatePlayer(0);
    player.Get<Transform>().Scale = glm::vec3(2.0f);

    auto& eModel = player.Add<EModel>();
    eModel.Model = TestModel.get();

    Players.push_back(player.GetHandler());

    CMD_LOG("] Entities initialized", INFO);
    return true;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Entities::Update()
{
    if (!IsRunning()) return;

    Physics::UpdateEntityPhysics(m_registry);

    if (g_Input->IsKeyPressed(SDL_SCANCODE_B))
    {
        auto player = CreatePlayer(0);
        player.Get<Transform>().Scale = glm::vec3(2.0f);

        auto& model = player.Add<EModel>();
        model.Model = TestModel.get();
        Players.push_back(player.GetHandler());
    }

    if (g_Input->IsKeyPressed(SDL_SCANCODE_Y))
    {
        for (auto& entt : Players)
        {
            Entity entity(m_registry, entt);
            entity.Destroy();
        }
        Players.clear();
    }
}

void Entities::Render()
{
    if (!IsRunning()) return;

    auto view = m_registry.view<Transform, EModel>();
    for (auto [entity, transform, renderable] : view.each())
    {
        auto& model = renderable.Model;

        if (!model) 
            continue;

        model->SetTransform(transform);
        model->Draw();
    }

    //for (auto& player : Players)
    //{
    //    auto& playerTrans = player.Get<Transform>();
    //    Gizmo::DrawBox(playerTrans.Position, playerTrans.Rotation, glm::vec3(1.0f), glm::vec3(-1.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    //    //Gizmo::DrawSphere(playerTrans.Position, playerTrans.Rotation, 1.0f, 8, 16);
    //}

    //Gizmo::DrawTriangle(
    //    glm::vec3(0.0f, 5.0f, 0.0f),
    //    glm::vec3(0.0f, 0.0f, 0.0f), 
    //    glm::vec3(1.0f, 0.0f, 0.0f), 
    //    glm::vec3(0.0f, 1.0f, 0.0f));

    //Gizmo::DrawBox(glm::vec3(), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(10.0f, 1.0f, 10.0f), glm::vec3(-10.0f, -1.0f, -10.0f));
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Entities::DestroyAll()
{
    m_registry.clear();
    g_Physics->FlushPendingDestructions();
}

void Entities::Shutdown()
{
    m_isRunning = false;
    DestroyAll();

    m_UUIDs.clear();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Entity Entities::CreateEntity()
{
    auto entity = m_registry.create();
    auto uuid = boost::uuids::random_generator()();

    m_registry.emplace<EEntityID>(entity, EEntityID{ uuid });
    m_UUIDs[uuid] = entity;
    return Entity(m_registry, entity);
}

std::optional<Entity> Entities::GetEntityByUUID(const UUID& uuid)
{
    auto it = m_UUIDs.find(uuid);

    if (it != m_UUIDs.end())
        return Entity(m_registry, it->second);

    return std::nullopt;
}