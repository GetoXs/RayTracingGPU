#pragma once
#include "MetaTypes.h"
#include "Ray.h"

//Klasa z funkcjonalno�ci� kamery ortogonalnej.
class CameraOrto
{
public:
	//Pozycja oka.
	Vector3D EyePosition;
	//Macierz projekcji.
	Matrix4x4 ProjectionMatrix;
	//Wielko�c okna.
	PointI Size;
	//K�t patrzenia.
	float Angle;

	//Metoda zwracaj�ca promie� od kamery.
	Ray GetRay(PointI screenLocation);

	CameraOrto(Vector3D eye, PointI size, float left, float right, float bottom, float top, float nearPlane, float farPlane) 
		: EyePosition(eye), Size(size), ProjectionMatrix()
	{
		ProjectionMatrix.setToIdentity();
		ProjectionMatrix.ortho(left, right, bottom, top, nearPlane, farPlane);
	}
};

