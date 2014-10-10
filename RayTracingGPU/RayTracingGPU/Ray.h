#pragma once
#include "MetaTypes.h"

class Ray
{
public:
	Vector3D Start;
	Vector3D Direction;

	Ray(Vector3D start, Vector3D direction) :Start(start), Direction(direction)
	{}
};

