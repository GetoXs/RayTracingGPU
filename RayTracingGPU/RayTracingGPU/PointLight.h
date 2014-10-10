#pragma once
#include "MetaTypes.h"
class PointLight
{
public:
	Color LightDiffuseColor;
	Color LightAmbientColor;
	Vector3D Position;

	PointLight(Vector3D position, Color diffuseColor, Color ambientColor) : Position(position),
		LightDiffuseColor(diffuseColor), LightAmbientColor(ambientColor)
	{
	}
};

