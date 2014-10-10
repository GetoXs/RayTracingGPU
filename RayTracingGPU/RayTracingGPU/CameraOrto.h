#pragma once
#include "MetaTypes.h"
#include "Ray.h"
class CameraOrto
{
public:
	Vector3D EyePosition;
	Matrix4x4 ProjectionMatrix;
	PointI Size;
	float Angle;

	Ray GetRay(PointI screenLocation);

	CameraOrto(Vector3D eye, PointI size, float left, float right, float bottom, float top, float nearPlane, float farPlane) 
		: EyePosition(eye), Size(size), ProjectionMatrix()
	{
		ProjectionMatrix.setToIdentity();
		ProjectionMatrix.ortho(left, right, bottom, top, nearPlane, farPlane);
	}
};

