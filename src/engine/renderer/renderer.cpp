#include <glad/glad.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "engine.hpp"
#include "renderer.hpp"
#include "singleton.hpp"
#include "cmd.hpp"
#include "mesh.hpp"
#include "shader_program.hpp"

static std::unique_ptr<Mesh> TestMesh;
static std::unique_ptr<ShaderProgram> TestShader;

void Renderer::Init(GLFWwindow* window)
{
    m_window = window;

    int width, height;
    glfwGetFramebufferSize(m_window, &width, &height);
    glViewport(0, 0, width, height);

    m_Camera = std::make_unique<RenderCamera>(width, height);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    const char* version = (const char*)glGetString(GL_VERSION);
    CMD_LOG("OpenGL Version: " + std::string(version), CMDLogType::INFO);

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

    TestMesh = std::make_unique<Mesh>(vertices, indices);
    TestShader = std::make_unique<ShaderProgram>();
}

bool Renderer::LoadGLLoader()
{
    return gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
}

void Renderer::Draw()
{
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    if (TestMesh)
    {
        TestShader->Bind();
        TestShader->SetMatrix4("model", TestMesh->GetTransformMatrix());
        TestShader->SetMatrix4("view", m_Camera->GetViewMatrix());
        TestShader->SetMatrix4("projection", m_Camera->GetProjectionMatrix());
        TestMesh->Draw();
        //TestShader->Unbind();
    }

    DrawGUI();

    glFinish();
}

void Renderer::DrawGUI()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    if (!m_firstFrame)
    {
        m_firstFrame = true;
        auto& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    }

    ImGui::NewFrame();

    auto panels = Engine::GetInstance().GetPanels();
    for (auto panel : panels)
    {
        if (panel)
            panel->Draw();
    }

    ImGui::ShowDemoWindow();
    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Renderer::OnWindowResize(int width, int height)
{
    glViewport(0, 0, width, height);
}

void Renderer::OnWindowClose()
{

}