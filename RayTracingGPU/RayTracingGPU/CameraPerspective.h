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

	Ray GetRay(PointI pixelLocation, RectI viewport);

	void RecalcModelViewProjectionMatrix()
	{
		this->_ModelViewProjectionMatrix = this->_ProjectionMatrix * this->_ModelViewMatrix;
		this->_ModelViewProjectionMatrixInverted = this->_ModelViewProjectionMatrix.inverted();
	}

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

#pragma region ModelView transformacje
	void ModelViewTranslate(float x, float y, float z)
	{
		this->_ModelViewMatrix.translate(x, y, z);
		this->RecalcModelViewProjectionMatrix();
	}
	void ModelViewRotate(float angle, float x, float y, float z)
	{
		this->_ModelViewMatrix.rotate(angle, x, y, z);
		this->RecalcModelViewProjectionMatrix();
	}
	void ModelViewRotate(float angle, const Vector3D *axis)
	{
		this->_ModelViewMatrix.rotate(angle, *axis);
		this->RecalcModelViewProjectionMatrix();
	}
	void ModelViewScale(float scale)
	{
		this->_ModelViewMatrix.scale(scale);
		this->RecalcModelViewProjectionMatrix();
	}
#pragma endregion

#pragma region Ctors
	CameraPerspective(float left, float right, float bottom, float top, float nearPlane, float farPlane)
		:_ModelViewMatrix()
	{
		this->_ProjectionMatrix.setToIdentity();
		this->_ProjectionMatrix.frustum(left, right, bottom, top, nearPlane, farPlane);

		this->RecalcModelViewProjectionMatrix();
	}
#pragma endregion
};

