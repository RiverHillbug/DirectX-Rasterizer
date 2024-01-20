#pragma once
#include "Math.h"
#include "Vector3.h"
#include "Vector3.h"

struct Vertex
{
	dae::Vector3 position{};
	dae::Vector2 uv{};
};

struct Vertex_Out
{
	dae::Vector4 position{};
	dae::Vector2 uv{};
};

enum class PrimitiveTopology
{
	TriangleList,
	TriangleStrip
};