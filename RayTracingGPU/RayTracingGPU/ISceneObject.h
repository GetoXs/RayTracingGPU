#pragma once
#include "Ray.h"
#include "ObjectHitResult.h"
#include "IMaterial.h"


class ISceneObject
{
public:
	unsigned int MaterialIndex;
	const IMaterial *Material;
	virtual bool HitTest(const Ray *ray, ObjectHitResult *outResult) = 0;
	Matrix4x4 ModelMatrix;
	Vector3D Position;

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
};

