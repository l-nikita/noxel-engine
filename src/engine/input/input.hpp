#pragma once

#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include <unordered_map>

#include "iinput.hpp"
#include "common/singleton.hpp"

class Input : public IInput
{
public:
	bool Init() override;
	bool IsKeyDown(SDL_Scancode key) override;
	bool IsKeyPressed(SDL_Scancode key) override;
	bool IsKeyReleased(SDL_Scancode key) override;

	bool IsButtonDown(Uint8 button) override;
	bool IsButtonPressed(Uint8 button) override;
	bool IsButtonReleased(Uint8 button) override;

	void Reset() override;

	void SetRelativeMouseMode(bool value) override;
	bool GetRelativeMouseMode() override;

	glm::vec2 GetMousePos() override;
	glm::vec2 GetMouseDelta() override;
	glm::vec2 GetMouseScroll() override;

	void OnKey(SDL_KeyboardEvent keyEvent) override;
	void OnMouseMotion(SDL_MouseMotionEvent mouseEvent) override;
	void OnMouseButton(SDL_MouseButtonEvent buttonEvent) override;
	void OnMouseWheel(SDL_MouseWheelEvent wheelEvent) override;

private:
	glm::vec2 m_scrollDelta;

	std::unordered_map<SDL_Scancode, bool> m_keyDown;
	std::unordered_map<SDL_Scancode, bool> m_keyPressed;
	std::unordered_map<SDL_Scancode, bool> m_keyReleased;

	std::unordered_map<Uint8, bool> m_mouseDown;
	std::unordered_map<Uint8, bool> m_mousePressed;
	std::unordered_map<Uint8, bool> m_mouseReleased;
};