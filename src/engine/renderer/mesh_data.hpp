#pragma once

#include <vector>

struct MeshData
{
	const std::vector<float> Vertices;
	const std::vector<int> Indices;
	const std::vector<float> UV;

	MeshData(std::vector<float> vertices, std::vector<int> indices, std::vector<float> uv)
		: Vertices(vertices), Indices(indices), UV(uv) {}
};