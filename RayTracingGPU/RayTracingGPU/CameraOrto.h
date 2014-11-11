#pragma once
#include "MetaTypes.h"
#include "Ray.h"

//Klasa z funkcjonalnoœci¹ kamery ortogonalnej.
class CameraOrto
{
public:
	//Pozycja oka.
	Vector3D EyePosition;
	//Macierz projekcji.
	Matrix4x4 ProjectionMatrix;
	//Wielkoœc okna.
	PointI Size;
	//K¹t patrzenia.
	float Angle;

	//Metoda zwracaj¹ca promieñ od kamery.
	Ray GetRay(PointI screenLocation);

	CameraOrto(Vector3D eye, PointI size, float left, float right, float bottom, float top, float nearPlane, float farPlane) 
		: EyePosition(eye), Size(size), ProjectionMatrix()
	{
		ProjectionMatrix.setToIdentity();
		ProjectionMatrix.ortho(left, right, bottom, top, nearPlane, farPlane);
	}
};

