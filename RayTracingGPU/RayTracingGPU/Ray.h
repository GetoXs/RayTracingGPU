#pragma once
#include "MetaTypes.h"

//Klasa z informacjami o promieniu.
class Ray
{
public:
	//Punkt startu promienia.
	Vector3D Start;
	//Wektor kierunku promienia.
	Vector3D Direction;

	Ray(Vector3D start, Vector3D direction) :Start(start), Direction(direction)
	{}
};

