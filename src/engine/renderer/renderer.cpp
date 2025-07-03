#include <glad/glad.h>
#include <SDL3/SDL.h>

#include "engine.hpp"
#include "gui/gui.hpp"
#include "assetsystem/assets_system.hpp"
#include "entities/entities.hpp"
#include "renderer.hpp"
#include "cmd/cmd.hpp"
#include "physics/physics.hpp"
#include "render_camera.hpp"

#include "mesh.hpp"
#include "model.hpp"
#include "gizmos.hpp"

static std::unique_ptr<Model> TestModel;

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool Renderer::Init()
{
    if (!InitAPI()) return false;

    int width, height;
    SDL_GetWindowSizeInPixels(g_Engine->GetWindow(), &width, &height);
    OnWindowResize(width, height);

    m_camera = new RenderCamera(width, height);

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
    TestModel->m_Transform.Scale = glm::vec3();

    CMD_LOG("] Renderer initialized", INFO);
    return true;
}

bool Renderer::InitAPI()
{
    m_Context = SDL_GL_CreateContext(g_Engine->GetWindow());
    if (!m_Context)
    {
        CMD_LOG("SDL_GL_CreateContext Error: " + std::string(SDL_GetError()), ERROR);
        return false;
    }

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
    {
        CMD_LOG("Failed to initialize GLAD!", ERROR);
        return false;
    }

    CMD_LOG("//----------------------------------", DEFAULT);
    const char* vendor = (const char*)glGetString(GL_VENDOR);
    const char* renderer = (const char*)glGetString(GL_RENDERER);
    const char* version = (const char*)glGetString(GL_VERSION);
    CMD_LOG("GPU: " + std::string(renderer), INFO);
    CMD_LOG("GPU Vendor: " + std::string(vendor), INFO);
    CMD_LOG("OpenGL Version: " + std::string(version), INFO);
    CMD_LOG("//----------------------------------", DEFAULT);

    SDL_GL_SetSwapInterval(1);

    glFrontFace(GL_CW);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return true;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Renderer::Render()
{
#ifdef _DEBUG
    m_DrawCallCount = 0;
#endif

    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Gizmo::DrawWorldCoords(glm::vec3());
    Gizmo::DrawGrid(glm::vec3(0.0f, -1.0f, 0.0f), glm::quat(), 20, 10);

    if (TestModel)
        TestModel->Draw();

    g_Entities->Render();
    g_Physics->Render(); // for debug purposes
    g_Gui->Render();

    SDL_GL_SwapWindow(g_Engine->GetWindow());
}

void Renderer::Shutdown()
{
    delete m_camera;
    SDL_GL_DestroyContext(m_Context);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Renderer::OnWindowResize(int width, int height)
{
    glViewport(0, 0, width, height);
}