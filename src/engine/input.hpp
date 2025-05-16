#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "singleton.hpp"

class Input : public Singleton<Input>
{
public:
	bool Init(GLFWwindow* window);
	bool IsKeyPressed(int key);
	bool IsKeyReleased(int key);
	bool IsButtonPressed(int button);
	bool IsButtonReleased(int button);

	void SetCursorMode(int mode);
	int GetCursorMode();

	glm::vec2 GetMousePos();
	glm::vec2 GetMouseDelta();
	glm::vec2 GetMouseScroll() const;

private:
	GLFWwindow* m_window;
	glm::vec2 m_lastMousePos;
	glm::vec2 m_scrollDelta;

	static void Callback_Key(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void Callback_CharMods(GLFWwindow* window, unsigned int codepoint, int mods);
	static void Callback_MouseMove(GLFWwindow* window, double xpos, double ypos);
	static void Callback_MouseButton(GLFWwindow* window, int button, int action, int mods);
	static void Callback_Scroll(GLFWwindow* window, double xoffset, double yoffset);
};