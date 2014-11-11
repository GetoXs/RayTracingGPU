#pragma once
#include "MetaTypes.h"
#include "IMaterial.h"
///Klasa zawieraj¹ca informacje o trafionym punkcie.
class HitResult
{
public:
	///Wynik dzia³ania.
	bool Result;
	///Trafiony materia³.
	IMaterial *HitMaterial;
	///Trafiony punkt.
	Vector3D HitPoint;
	///Normalna w punkcie.
	Vector3D HitNormal;
	
	HitResult() :Result()
	{
	}
};

