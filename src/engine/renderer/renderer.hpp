#pragma once

#include <memory>

#include <GLFW/glfw3.h>
#include "common.hpp"
#include "singleton.hpp"
#include "render_camera.hpp"

class Renderer : public Singleton<Renderer>
{
public:
	std::unique_ptr<RenderCamera> m_Camera;

	void Init(GLFWwindow* window);
	bool LoadGLLoader();
	void Draw();
	void DrawGUI();
	void OnWindowResize(int width, int height);
	void OnWindowClose();

private:
	GLFWwindow* m_window = nullptr;
	bool m_firstFrame = false;
};