#include <glad/glad.h>
#include <SDL3/SDL.h>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_opengl3.h>

#include "engine.hpp"
#include "assets_system.hpp"
#include "entities.hpp"
#include "renderer.hpp"
#include "singleton.hpp"
#include "cmd.hpp"

#include "mesh.hpp"
#include "model.hpp"
#include "gizmos.hpp"

static std::unique_ptr<Model> TestModel;

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool Renderer::Init(SDL_Window* window)
{
    m_window = window;

    int width, height;
    SDL_GetWindowSizeInPixels(m_window, &width, &height);
    OnWindowResize(width, height);

    m_Camera = std::make_unique<RenderCamera>(width, height);

    InitAPI();
    InitGUI();

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

    //auto grid = Gizmo::Grid::Create(16, 5.0f, glm::vec3(1.0f));
    //grid->m_Transform.Position = glm::vec3(-2.5f, -1.0f, -2.5f);

    //Gizmo::WorldCoords::Create(Transform{});

    //glm::vec3 min(-0.5f, -0.5f, -0.5f);
    //glm::vec3 max(0.5f, 0.8f, 0.5f);
    //Gismo::Box::Create(min, max, glm::vec3(1.0f));

    CMD_LOG("] Renderer initialized", INFO);
    return true;
}

void Renderer::InitAPI()
{
    CMD_LOG("//----------------------------------", DEFAULT);
    const char* vendor = (const char*)glGetString(GL_VENDOR);
    const char* renderer = (const char*)glGetString(GL_RENDERER);
    const char* version = (const char*)glGetString(GL_VERSION);
    CMD_LOG("GPU: " + std::string(renderer), CMDLogType::INFO);
    CMD_LOG("GPU Vendor: " + std::string(vendor), CMDLogType::INFO);
    CMD_LOG("OpenGL Version: " + std::string(version), CMDLogType::INFO);
    CMD_LOG("//----------------------------------", DEFAULT);

    SDL_GL_SetSwapInterval(1);

    glFrontFace(GL_CW);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
}

void Renderer::InitGUI()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui::StyleColorsDark();

    ImGui_ImplSDL3_InitForOpenGL(m_window, Engine::GetInstance().m_Context);
    ImGui_ImplOpenGL3_Init("#version 330");
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Renderer::Draw()
{
#ifdef _DEBUG
    m_DrawCallCount = 0;
#endif

    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (TestModel)
    {
        TestModel->Draw();
        //auto deltaTime = Engine::GetInstance().DeltaTime() / 1000;
        //test += deltaTime * 10.0f;
        //TestModel->m_Transform.Rotation = Angles(0, test, 0);
    }

    Entities::GetInstance().DrawAll();

    DrawGUI();
}

void Renderer::DrawGUI()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();

    ImGui::NewFrame();

    auto panels = Engine::GetInstance().GetPanels();
    for (auto panel : panels)
    {
        if (panel && panel->m_IsVisible)
            panel->Draw();
    }

    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Renderer::OnWindowResize(int width, int height)
{
    glViewport(0, 0, width, height);
}

void Renderer::OnWindowClose()
{

}