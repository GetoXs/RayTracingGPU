#pragma once
#include "MetaTypes.h"
#include "IMaterial.h"
class HitResult
{
public:
	bool Result;
	IMaterial *HitMaterial;
	Vector3D HitPoint;
	Vector3D HitNormal;
	
	HitResult() :Result()
	{
	}
};

