#pragma once
#include "MetaTypes.h"
#include "IMaterial.h"
///Klasa zawierająca informacje o trafionym punkcie.
class HitResult
{
public:
	///Wynik działania.
	bool Result;
	///Trafiony materiał.
	IMaterial *HitMaterial;
	///Trafiony punkt.
	Vector3D HitPoint;
	///Normalna w punkcie.
	Vector3D HitNormal;
	
	HitResult() :Result()
	{
	}
};

