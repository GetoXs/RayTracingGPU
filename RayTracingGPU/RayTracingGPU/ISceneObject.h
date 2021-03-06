#pragma once
#include "Ray.h"
#include "ObjectHitResult.h"
#include "IMaterial.h"

///Klasa abstrakcyjna zawierająca informacje o obiekcie sceny.
class ISceneObject
{
public:
	///Index materiału przypisanego obiektowi.
	unsigned int MaterialIndex;
	///Przypisany materiał.
	const IMaterial *Material;
	///Funkcja testująca trafienie promienia w obiekt.
	virtual bool HitTest(const Ray *ray, ObjectHitResult *outResult) = 0;
	///Macierz modelu.
	Matrix4x4 ModelMatrix;
	///Pozycja obiektu.
	Vector3D Position;

	///Funkcja przypisująca materiał do obiektu.
	virtual void AssignMaterial(const IMaterial *material, const unsigned int matIndex)
	{
		this->Material = material;
		this->MaterialIndex = matIndex;
	}
	virtual void SetPosition(const Vector3D *dstPosition)
	{
		this->ModelMatrix.setToIdentity();
		this->ModelMatrix.translate(*dstPosition);
		this->Position = *dstPosition;
	}

#pragma region Ctors Dtors
	ISceneObject() : Position()
	{
		this->ModelMatrix.setToIdentity();
	}
	ISceneObject(const Vector3D *dstPosition) :ISceneObject()
	{
		this->SetPosition(dstPosition);
	}
	~ISceneObject()
	{
		if(this->Material)
			delete(this->Material);
		this->Material = NULL;
	}
#pragma endregion
};

