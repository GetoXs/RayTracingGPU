#pragma once
#include "Mesh.h"
#include "qlist.h"
#include "HitResult.h"
#include "qvector.h"

///Klasa z funkcjonalno�ci� sceny.
class Scene
{
private:
	///Lista obiekt�w sceny.
	QList<ISceneObject*> ObjectList;
	///Lista specjalnych obiekt�w meshy sceny.
	QList<Mesh*> MeshObjectList;
	///Lista obiekt�w materia��w.
	QList<IMaterial*> MaterialList;

	//Aktualny offset dla indexu materia��w.
	unsigned MeshIndexOffset;

public:
	///Obs�uga testowania promienia.
	HitResult HandleRay(const Ray *ray);

#pragma region Addy
	void AddObject(ISceneObject *object, IMaterial *newMaterial)
	{
		this->AddObject(object, this->AddMaterial(newMaterial));
	}
	void AddObject(ISceneObject *object, unsigned materialIndex)
	{
		Mesh *mesh = dynamic_cast<Mesh*> (object);
		if (mesh != NULL)
		{	//w przypadku gdy to mesh dodajemy takze do drugiej tablicy
			this->AddMeshObject(mesh, materialIndex);
		}
		else
		{
			object->AssignMaterial(MaterialList[materialIndex], materialIndex);
		}
		this->ObjectList.append(object);
	}
	void AddMeshObject(Mesh *mesh, unsigned materialIndex)
	{
		mesh->AssignMaterial(MaterialList[materialIndex], materialIndex);
		this->MeshIndexOffset += mesh->PositionCount;
		this->MeshObjectList.append(mesh);
	}
	unsigned AddMaterial(IMaterial *material)
	{
		this->MaterialList.append(material);
		//index ostatniego
		return this->MaterialList.count() - 1;
	}
#pragma endregion

#pragma region Gety
	unsigned *GetPositionIndexData(unsigned *count, bool withPositionOffset)
	{
		unsigned indexOffset = 0;
		std::vector<unsigned> *returnArray = new std::vector<unsigned>();
		for each (Mesh *mesh in this->MeshObjectList)
		{
			//dodanie dodatkowego offsetu przy pobieraniu pe�nej tablicy index�w
			//wymagane przy pobieraniu pelnej tablicy pozycji oraz indexow
			unsigned *tmpIndexData = mesh->IndexData;
			if (indexOffset != 0 && withPositionOffset)
			{
				for (int i = 0; i < mesh->IndexDataCount; i++)
					returnArray->push_back(tmpIndexData[i] + indexOffset);
			}
			else
			{	//dla pierwszego elementu (offset = 0)
				returnArray->insert(returnArray->end(), tmpIndexData, (tmpIndexData + mesh->IndexDataCount));
			}
			//offset zwi�kszony o liczbe zdefiniowanych wierzcho�k�w
			indexOffset += mesh->PositionCount;

		}
		*count = returnArray->size();
		return returnArray->data();
	}
	float *GetPositionData(unsigned *count)
	{
		std::vector<float> *returnArray = new std::vector<float>();
		for each (Mesh *mesh in this->MeshObjectList)
		{
			returnArray->insert(returnArray->end(), mesh->PositionData, (mesh->PositionData + mesh->PositionDataCount));
		}
		*count = returnArray->size();
		return returnArray->data();
	}
	unsigned *GetNormalIndexData(unsigned *count)
	{
		std::vector<unsigned> *returnArray = new std::vector<unsigned>();
		for each (Mesh *mesh in this->MeshObjectList)
		{
			returnArray->insert(returnArray->end(), mesh->IndexData, (mesh->IndexData + mesh->IndexDataCount));
		}
		*count = returnArray->size();
		return returnArray->data();
	}
	float *GetNormalData(unsigned *count)
	{
		std::vector<float> *returnArray = new std::vector<float>();
		for each (Mesh *mesh in this->MeshObjectList)
		{
			returnArray->insert(returnArray->end(), mesh->NormalData, (mesh->NormalData + mesh->PositionDataCount));
		}
		*count = returnArray->size();
		return returnArray->data();
	}
	unsigned *GetMaterialIndexData(unsigned *count)
	{
		std::vector<unsigned> *returnArray = new std::vector<unsigned>();
		for each (Mesh *mesh in this->MeshObjectList)
		{
			returnArray->insert(returnArray->end(), mesh->MaterialIndexData, (mesh->MaterialIndexData + mesh->TriangleCount));
		}
		*count = returnArray->size();
		return returnArray->data();
	}
	float *GetMaterialPropertiesData(unsigned *count)
	{
		std::vector<float> *returnArray = new std::vector<float>();
		for each (IMaterial *mat in this->MaterialList)
		{
			unsigned int tmpCount;
			float *tmpMatData = mat->GetColorProperties(&tmpCount);
			returnArray->insert(returnArray->end(), tmpMatData, (tmpMatData + tmpCount));
		}
		*count = returnArray->size();
		return returnArray->data();
	}
	unsigned GetPositionCount()
	{
		unsigned result = 0;
		for each (Mesh *mesh in this->MeshObjectList)
			result += mesh->PositionCount;
		return result;
	}
	unsigned GetTriangleCount()
	{
		unsigned result = 0;
		for each (Mesh *mesh in this->MeshObjectList)
			result += mesh->TriangleCount;
		return result;
	}
#pragma endregion

#pragma region Ctors
	Scene() : ObjectList()
	{
		MeshIndexOffset = 0;
	}
#pragma endregion
};

