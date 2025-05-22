#pragma once

#include <vector>
#include <glad/glad.h>

struct MeshData
{
	const std::vector<float> Vertices;
	const std::vector<int> Indices;
	const std::vector<float> UV;
	const GLenum PrimitiveType;

	MeshData(std::vector<float> vertices, std::vector<int> indices, std::vector<float> uv, GLenum primitiveType)
		: Vertices(vertices), Indices(indices), UV(uv), PrimitiveType(primitiveType) {}
};