#pragma once

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
struct IViewport
{
	int X = 0;
	int Y = 0;
	int Width = 0;
	int Height = 0;
	float MinDepth = 0.0f;
	float MaxDepth = 1.0f;

	IViewport(int x, int y, int width, int height, float minDepth = 0.0f, float maxDepth = 1.0f)
		: X(x), Y(y), Width(width), Height(height), MinDepth(minDepth), MaxDepth(maxDepth)
	{
	}

	void Resize(int width, int height)
	{
		Width = width;
		Height = height;
	}
};