#include "stdafx.h"
#include "Mesh.h"
#include <assert.h>
#define ASSIMP_DLL
#include <Importer.hpp>
#include <Exporter.hpp>
#include <postprocess.h>
#include <assimp\scene.h>

void Mesh::_load(const char* filename, bool flipFaces)
{
	Assimp::Importer importer;
	unsigned flags = aiProcess_GenNormals | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices;
	if (flipFaces)
		flags |= aiProcess_FlipWindingOrder;
	const aiScene* sc = importer.ReadFile(filename, flags);
	if (sc == NULL)
	{
		const char* erer = importer.GetErrorString();
		throw std::exception("Mesh:_load error: null scene");
	}
	const aiMesh* mesh = sc->mMeshes[0];

	//konwersja struktury assimp na tablice
	unsigned int *indexArray;
	indexArray = new unsigned int[mesh->mNumFaces * 3];
	unsigned int faceIndex = 0;
	for (unsigned int t = 0; t < mesh->mNumFaces; ++t)
	{
		const aiFace* face = &mesh->mFaces[t];
		memcpy(&indexArray[faceIndex], face->mIndices, 3 * sizeof(unsigned int));
		faceIndex += 3;
	}

	PositionCount = mesh->mNumVertices;
	TriangleCount = mesh->mNumFaces;
	IndexDataCount = TriangleCount * 3;
	PositionDataCount = PositionCount * 3;
	IndexData = indexArray;
	PositionData = new float[PositionDataCount];
	memcpy(PositionData, mesh->mVertices, PositionDataCount * sizeof(float));
	NormalData = NULL;
	NormalData = new float[PositionDataCount];
	memcpy(NormalData, mesh->mNormals, PositionDataCount * sizeof(float));
}

void Mesh::_loadMaterial(const unsigned int matIndex)
{
	assert(PositionCount > 0 && TriangleCount > 0);

	unsigned int *materialIndexArray = new unsigned int[this->TriangleCount];
	for (int i = 0; i < this->TriangleCount; i++)
	{
		materialIndexArray[i] = this->MaterialIndex;
	}
	MaterialIndexData = materialIndexArray;
}