#pragma once
#include "ISceneObject.h"

class Sphere 
	//:public ISceneObject
{
public:
	Vector3D Center;
	double Radius;

	virtual ObjectHitResult HitTest(Ray *ray);

	Sphere(Vector3D center, double radius)
//		:ISceneObject(), Center(center), Radius(radius)
	{
	}
};

