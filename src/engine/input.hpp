#pragma once

#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include <unordered_map>

#include "singleton.hpp"

class Input : public Singleton<Input>
{
public:
	bool Init(SDL_Window* window);
	bool IsKeyDown(SDL_Scancode key);
	bool IsKeyPressed(SDL_Scancode key);
	bool IsKeyReleased(SDL_Scancode key);

	bool IsButtonDown(Uint8 button);
	bool IsButtonPressed(Uint8 button);
	bool IsButtonReleased(Uint8 button);

	void Reset();

	void SetRelativeMouseMode(bool value);
	bool GetRelativeMouseMode();

	glm::vec2 GetMousePos();
	glm::vec2 GetMouseDelta();
	glm::vec2 GetMouseScroll() const;

	void OnKey(SDL_KeyboardEvent keyEvent);

	void OnMouseMotion(SDL_MouseMotionEvent mouseEvent);
	void OnMouseButton(SDL_MouseButtonEvent buttonEvent);
	void OnMouseWheel(SDL_MouseWheelEvent wheelEvent);

private:
	SDL_Window* m_window;
	glm::vec2 m_scrollDelta;

	std::unordered_map<SDL_Scancode, bool> m_keyDown;
	std::unordered_map<SDL_Scancode, bool> m_keyPressed;
	std::unordered_map<SDL_Scancode, bool> m_keyReleased;

	std::unordered_map<Uint8, bool> m_mouseDown;
	std::unordered_map<Uint8, bool> m_mousePressed;
	std::unordered_map<Uint8, bool> m_mouseReleased;
};