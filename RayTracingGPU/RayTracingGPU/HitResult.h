#pragma once
#include "MetaTypes.h"
#include "IMaterial.h"
///Klasa zawieraj�ca informacje o trafionym punkcie.
class HitResult
{
public:
	///Wynik dzia�ania.
	bool Result;
	///Trafiony materia�.
	IMaterial *HitMaterial;
	///Trafiony punkt.
	Vector3D HitPoint;
	///Normalna w punkcie.
	Vector3D HitNormal;
	
	HitResult() :Result()
	{
	}
};

