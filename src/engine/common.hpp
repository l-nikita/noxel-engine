#pragma once

constexpr auto ENGINE_NAME = "Noxel Engine";

#ifdef ENGINE_EXPORTS
	#define ENGINE_API __declspec(dllexport)
#else
	#define ENGINE_API __declspec(dllimport)
#endif