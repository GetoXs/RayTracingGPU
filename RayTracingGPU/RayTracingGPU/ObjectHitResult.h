#pragma once
#include "MetaTypes.h"
class ObjectHitResult
{
public:
	bool Result;
	double MinDistance;
	Vector3D HitNormal;
	Vector3D HitPoint;

	ObjectHitResult() :Result(false), MinDistance(DBL_MAX), HitNormal()
	{
	}
};

