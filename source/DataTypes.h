#pragma once
#include "Math.h"
#include "Vector3.h"
#include "Vector3.h"

struct Vertex
{
	dae::Vector3 position{};
	dae::Vector3 color{};
};

struct Vertex_Out
{
	dae::Vector4 position{};
	dae::Vector3 color{};
};

enum class PrimitiveTopology
{
	TriangleList,
	TriangleStrip
};