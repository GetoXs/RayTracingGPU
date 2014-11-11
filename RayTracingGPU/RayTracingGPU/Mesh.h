#pragma once
#include "ISceneObject.h"
#include "IMaterial.h"
#include "Triangle3D.h"
#include <assert.h>

class Mesh :
	public ISceneObject
{
public:
	///Dane z indexem (wierzcholkow oraz normalnych)
	unsigned int *IndexData;
	///Dane z pozycj¹ wierzcho³ków
	float *PositionData;

	///Dane z wektorem normalnych
	float *NormalData;

	///Dane z indexem materia³u
	unsigned int *MaterialIndexData;

	///Liczba wierzcho³kow
	unsigned PositionCount;
	///Liczba wierzcholkow * 3 skladowe wektora
	unsigned PositionDataCount;
	///Liczba trójk¹tów * 3 indexy trójk¹ta
	unsigned IndexDataCount;
	///Liczba trojkatow
	unsigned TriangleCount;

	void GetTriangle(int triangleIndex, Triangle3D *outTriangle)
	{
		assert(triangleIndex <= this->TriangleCount);
		unsigned pi0, pi1, pi2;

		pi0 = this->IndexData[triangleIndex * 3 + 0];
		pi1 = this->IndexData[triangleIndex * 3 + 1];
		pi2 = this->IndexData[triangleIndex * 3 + 2];

		outTriangle->LoadFromFloats(
			this->PositionData + 3 * pi0,
			this->PositionData + 3 * pi1,
			this->PositionData + 3 * pi2);
	}
	///Funkcja implementuj¹ca test trafienia w trójk¹t podany parametrem.
	bool HitTestTriangle(const Ray *ray, const Triangle3D *testedTriangle, ObjectHitResult *outResult)
	{
		//wyliczenie krawêdzi
		Vector3D e0 = testedTriangle->V1 - testedTriangle->V0;
		Vector3D e1 = testedTriangle->V2 - testedTriangle->V0;

		//wyliczenie wektora normalnego do trojkata
		outResult->HitNormal= Vector3D::crossProduct(e0, e1);
		outResult->HitNormal.normalize();

		//wektor prostopad³y do plaszczyzny (kierunku promienia i krawêdzi)
		Vector3D s1 = Vector3D::crossProduct(ray->Direction, e1);

		//sprawdzamy czy istnieje przeciecie wektora z drug¹ krawêdzi¹
		float d = Vector3D::dotProduct(e0, s1);
		if (d == 0.0)
			return false;

		bool isFront = d > 0;
		if (!isFront)
			return false;

		float invD = 1.0 / d;

		//obliczanie pierwszej wspó³rzednej barycentrycznej
		Vector3D d1 = ray->Start - testedTriangle->V0;
		float bc0 = Vector3D::dotProduct(d1, s1) * invD;
		if (bc0 < 0.0 || bc0 > 1.0)
			return false;

		//obliczanie drugiej wspó³rzednej barycentrycznej
		Vector3D s2 = Vector3D::crossProduct(d1, e0);
		float bc1 = Vector3D::dotProduct(ray->Direction, s2) * invD;
		if (bc1 < 0.0 || (bc0 + bc1) > 1.0)
			return false;

		//kalkulacja dystansu do plaszczyzny
		outResult->MinDistance = Vector3D::dotProduct(e1, s2) * invD;

		//sprawdzenie czy trojkat jest po wlaœciwej stronie promienia
		if (outResult->MinDistance <= 0)
			return false;

		//punkt przeciêcia prostej z p³aszczyzn¹
		outResult->HitPoint = ray->Start + (ray->Direction * outResult->MinDistance);

		return true;
	}
	///Sprawdzanie przeciecia z meshem.
	virtual bool HitTest(const Ray *ray, ObjectHitResult *outResult)
	{
		Triangle3D testedTriangle;
		ObjectHitResult triangleResult;

		outResult->MinDistance = DBL_MAX;
		outResult->Result = false;
		for (float i = 0; i < this->TriangleCount; i++)
		{
			//pobranie trójk¹ta
			this->GetTriangle(i, &testedTriangle);
			//testowanie trójk¹ta w tym sprawdzenie czy odleg³osc jest mniejsza od aktualnie zapisanej
			if (this->HitTestTriangle(ray, &testedTriangle, &triangleResult)
				&& triangleResult.MinDistance < outResult->MinDistance)
			{
				//aktualizacja wyniku
				*outResult = triangleResult;
				outResult->Result = true;
			}
		}
		return outResult->Result;
	}
	///Przypisanie materia³u.
	void AssignMaterial(const IMaterial *material, const unsigned int matIndex)
	{
		ISceneObject::AssignMaterial(material, matIndex);
		this->_loadMaterial(matIndex);
	}

	///przesuniecie indexu (z powodu zmiany indexu w teskturze przechodzacej do shadera
	void ApplyIndexOffset(unsigned indexOffset)
	{
		if (indexOffset == 0)
			return;
		for (int i = 0; i < this->IndexDataCount; i++)
			IndexData[i] += indexOffset;
	}
#pragma region Przekszta³cenia afiniczne
	///przesuniecie pozycji wzglêdem ostatniej pozycji
	///depricated
	void SetPosition(const Vector3D *dstPosition)
	{
		Vector3D diff = *dstPosition - this->Position;

		for (int i = 0; i < this->PositionDataCount; i += 3)
		{
			this->PositionData[i + 0] += diff.x();
			this->PositionData[i + 1] += diff.y();
			this->PositionData[i + 2] += diff.z();
		}
		ISceneObject::SetPosition(dstPosition);
	}
	void ApplyTransform(const Matrix4x4 *transform)
	{
		Vector3D tmp;
		for (int i = 0; i < this->PositionDataCount; i += 3)
		{
			tmp = Vector3D(this->PositionData[i + 0], this->PositionData[i + 1], this->PositionData[i + 2]);
			tmp = *transform * tmp;
			this->PositionData[i + 0] = tmp.x();
			this->PositionData[i + 1] = tmp.y();
			this->PositionData[i + 2] = tmp.z();
		}
		this->ModelMatrix = *transform;
	}

	void Translate(const Vector3D *dstPosition)
	{
		Matrix4x4 modelMatrix;
		modelMatrix.translate(dstPosition->x(), dstPosition->y(), dstPosition->z());
		this->ApplyTransform(&modelMatrix);
	}
	void Scale(float ratio)
	{
		Matrix4x4 modelMatrix;
		modelMatrix.scale(ratio);
		this->ApplyTransform(&modelMatrix);
	}
	void Rotate(float angle, const Vector3D *vector)
	{
		Matrix4x4 modelMatrix;
		modelMatrix.rotate(angle, *vector);
		this->ApplyTransform(&modelMatrix);
	}
#pragma endregion

#pragma region Ctors, Dtors
	Mesh(const char* filename, const Vector3D *dstPosition)
		:Mesh(filename, dstPosition, 1.f, false)
	{
	}
	Mesh(const char* filename, const Vector3D *dstPosition, float scaleRatio)
		:Mesh(filename, dstPosition, scaleRatio, false)
	{
	}
	Mesh(const char* filename, const Vector3D *dstPosition, float scaleRatio, bool flipFaces)
		:ISceneObject()
	{
		IndexData = NULL;
		PositionData = NULL;
		NormalData = NULL;
		MaterialIndexData = NULL;
		TriangleCount = PositionDataCount = PositionCount = 0;

		this->_load(filename, flipFaces);

		this->Translate(dstPosition);
		this->Scale(scaleRatio);
	}

	~Mesh()
	{
		delete[] this->MaterialIndexData;
		MaterialIndexData = NULL;
		delete[] this->PositionData;
		PositionData = NULL;
		delete[] this->NormalData;
		NormalData = NULL;
	}
#pragma endregion

private:

#pragma region Loadery
	void _load(const char* filename, bool flipFaces);
	void _loadMaterial(const unsigned int matIndex);
#pragma endregion

};

