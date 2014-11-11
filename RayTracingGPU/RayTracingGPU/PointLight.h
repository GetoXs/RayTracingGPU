#pragma once
#include "MetaTypes.h"

//Klasa œwiat³a punktowego.
class PointLight
{
public:
	//Kolor rozproszenia.
	Color LightDiffuseColor;
	//Kolor otoczenia.
	Color LightAmbientColor;
	//Pozycja
	Vector3D Position;

	PointLight(Vector3D position, Color diffuseColor, Color ambientColor) : Position(position),
		LightDiffuseColor(diffuseColor), LightAmbientColor(ambientColor)
	{
	}
};

