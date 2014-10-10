#pragma once
#include "MetaTypes.h"
#include "Ray.h"

class CameraPerspective
{
public:
	Matrix4x4 ProjectionMatrix;
	Vector3D CameraPosition;
	double Distance;
	float NearPlane;
	float FarPlane;

	Ray GetRay(PointI pixelLocation, RectI viewport);

	CameraPerspective(Vector3D eyePosition, Vector3D lookPoint, Vector3D up, double distanse,
		float left, float right, float bottom, float top, float nearPlane, float farPlane
		) :ProjectionMatrix(), Distance(distanse), CameraPosition(eyePosition), NearPlane(nearPlane), FarPlane(farPlane)
	{
		this->ProjectionMatrix.setToIdentity();
		this->ProjectionMatrix.frustum(left, right, bottom, top, nearPlane, farPlane);
		//this->ProjectionMatrix.lookAt(eyePosition, lookPoint, up);
	}
};

