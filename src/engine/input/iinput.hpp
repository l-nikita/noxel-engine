#pragma once

#include <glm/glm.hpp>
#include <cstdint>

enum SDL_Scancode;
typedef uint8_t Uint8;

struct SDL_KeyboardEvent;
struct SDL_MouseMotionEvent;
struct SDL_MouseButtonEvent;
struct SDL_MouseWheelEvent;

class IInput
{
public:
	virtual ~IInput() = default;

	virtual bool Init() = 0;

	virtual bool IsKeyDown(SDL_Scancode key) = 0;
	virtual bool IsKeyPressed(SDL_Scancode key) = 0;
	virtual bool IsKeyReleased(SDL_Scancode key) = 0;

	virtual bool IsButtonDown(Uint8 button) = 0;
	virtual bool IsButtonPressed(Uint8 button) = 0;
	virtual bool IsButtonReleased(Uint8 button) = 0;

	virtual void Reset() = 0;

	virtual void SetRelativeMouseMode(bool value) = 0;
	virtual bool GetRelativeMouseMode() = 0;

	virtual glm::vec2 GetMousePos() = 0;
	virtual glm::vec2 GetMouseDelta() = 0;
	virtual glm::vec2 GetMouseScroll() = 0;

	virtual void OnKey(SDL_KeyboardEvent keyEvent) = 0;
	virtual void OnMouseMotion(SDL_MouseMotionEvent mouseEvent) = 0;
	virtual void OnMouseButton(SDL_MouseButtonEvent buttonEvent) = 0;
	virtual void OnMouseWheel(SDL_MouseWheelEvent wheelEvent) = 0;
};

extern IInput* g_Input;