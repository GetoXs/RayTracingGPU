#pragma once
#include "MetaTypes.h"
#include "Ray.h"

class CameraPerspective
{
private:
	Matrix4x4 _ModelViewMatrix;
	Matrix4x4 _ProjectionMatrix;
	Matrix4x4 _ModelViewProjectionMatrix;
	Matrix4x4 _ModelViewProjectionMatrixInverted;
public:

#pragma region Gety Sety
	const Matrix4x4* GetProjectionMatrix()
	{
		return &this->_ProjectionMatrix;
	}
	const Matrix4x4* GetModelViewProjectionMatrix()
	{
		return &this->_ModelViewProjectionMatrix;
	}
	const Matrix4x4* GetModelViewProjectionMatrixInverted()
	{
		return &this->_ModelViewProjectionMatrixInverted;
	}
	void SetProjectionMatrix(const Matrix4x4 *mat)
	{
		this->_ProjectionMatrix = *mat;
		this->RecalcModelViewProjectionMatrix();
	}
	void SetModelViewMatrix(const Matrix4x4 *mat)
	{
		this->_ModelViewMatrix = *mat;
		this->RecalcModelViewProjectionMatrix();
	}
#pragma endregion

	Ray GetRay(PointI pixelLocation, RectI viewport);

	void RecalcModelViewProjectionMatrix()
	{
		this->_ModelViewProjectionMatrix = this->_ProjectionMatrix * this->_ModelViewMatrix;
		this->_ModelViewProjectionMatrixInverted = this->_ModelViewProjectionMatrix.inverted();
	}

	CameraPerspective(float left, float right, float bottom, float top, float nearPlane, float farPlane) 
	{
		this->_ProjectionMatrix.setToIdentity();
		this->_ProjectionMatrix.frustum(left, right, bottom, top, nearPlane, farPlane);

		this->_ModelViewMatrix.setToIdentity();

		this->RecalcModelViewProjectionMatrix();
	}
};

