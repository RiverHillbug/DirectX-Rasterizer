#pragma once
#include "Math.h"
#include "Vector3.h"
#include "Vector3.h"

struct Vertex
{
	dae::Vector3 position{};
	dae::Vector2 uv{};
	dae::Vector3 normal{};
	dae::Vector3 tangent{};
};

struct Vertex_Out
{
	dae::Vector4 position{};
	dae::Vector2 uv{};
	dae::Vector3 normal{};
	dae::Vector3 tangent{};
};

enum class PrimitiveTopology
{
	TriangleList,
	TriangleStrip
};