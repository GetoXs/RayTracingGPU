#pragma once
#include "ISceneObject.h"
#include "IMaterial.h"

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
	//Liczba wierzcholkow * 3 skladowe wektora
	unsigned PositionDataCount;
	//Liczba trójk¹tów * 3 indexy trójk¹ta
	unsigned IndexDataCount;
	///Liczba trojkatow
	unsigned TriangleCount;

	ObjectHitResult HitTest(Ray *ray)
	{
		//TODO:
		return ObjectHitResult();
	}

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
	///przesuniecie pozycji wzglêdem ostatniej pozycji.ssssss
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

	Mesh(const char* filename, const Vector3D *dstPosition)
		:ISceneObject()
	{
		IndexData = NULL;
		PositionData = NULL;
		NormalData = NULL;
		MaterialIndexData = NULL;
		TriangleCount = PositionDataCount = PositionCount = 0;

		this->_load(filename);
		this->SetPosition(dstPosition);
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

private:
	void _load(const char* filename);
	void _loadMaterial(const unsigned int matIndex);

};

