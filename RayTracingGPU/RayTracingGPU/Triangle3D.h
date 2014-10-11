#pragma once
#include "MetaTypes.h"

class Triangle3D
{
public:
	Vector3D V0;
	Vector3D V1;
	Vector3D V2;

	Triangle3D()
	{

	}
	Triangle3D(const Vector3D *v0, const Vector3D *v1, const Vector3D *v2)
		:V0(*v0), V1(*v1), V2(*v2)
	{
	}
	Triangle3D(const float *v0, const float *v1, const float *v2)
		:V0(v0[0], v0[1], v0[2]), 
		V1(v1[0], v1[1], v1[2]),
		V2(v2[0], v2[1], v2[2])
	{

	}
	void LoadFromFloats(const float *v0, const float *v1, const float *v2)
	{
		this->V0.setX(v0[0]);
		this->V0.setY(v0[1]);
		this->V0.setZ(v0[2]);

		this->V1.setX(v1[0]);
		this->V1.setY(v1[1]);
		this->V1.setZ(v1[2]);

		this->V2.setX(v2[0]);
		this->V2.setY(v2[1]);
		this->V2.setZ(v2[2]);
	}
};

