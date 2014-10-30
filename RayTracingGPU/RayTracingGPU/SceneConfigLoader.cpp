#include "stdafx.h"
#include "SceneConfigLoader.h"
#include "GLMgr.h"
#include "StandardMaterial.h"


#pragma region Parse

bool SceneConfigLoader::Parse(GLMgr *mgr)
{
	QFile file;
	file.setFileName(this->_SceneConfigFilePath);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return false;
	QString txt = file.readAll();
	file.close();
	QJsonDocument jsonD = QJsonDocument::fromJson(txt.toUtf8());

	QJsonObject jsonObj = jsonD.object();

	this->ParseRayTracer(&jsonObj.value("rayTracer"), mgr);
	this->ParseCamera(&jsonObj.value("camera"), mgr);
	this->ParseMaterials(&jsonObj.value("materials"), mgr);
	this->ParseObjects(&jsonObj.value("objects"), mgr);
	this->ParseLights(&jsonObj.value("lights"), mgr);
	return true;
}


void SceneConfigLoader::ParseRayTracer(const QJsonValue *jsonRT, GLMgr *mgr)
{
	QJsonObject jsonObj = jsonRT->toObject();
	//g³êbokoœæ ray tracera
	float depth = jsonObj.value("depth").toDouble();

	mgr->RayTracerDepth = depth;
}

void SceneConfigLoader::ParseCamera(const QJsonValue *jsonCam, GLMgr *mgr)
{
	QJsonObject jsonObj = jsonCam->toObject();

	if (jsonObj.contains("rotate"))
	{
		//rotacja kamery
		QJsonObject jsonRot = jsonObj.value("rotate").toObject();
		float angle = jsonRot["angle"].toDouble();
		Vector3D axis = SceneConfigLoader::JsonArrayToVector3D(&jsonRot["axis"].toArray());
		mgr->Camera->ModelViewRotate(angle, &axis);
	}

}
void SceneConfigLoader::ParseMaterials(const QJsonValue *jsonMats, GLMgr *mgr)
{
	for each (QJsonValue val in jsonMats->toArray())
	{
		QJsonObject jsonObj = val.toObject();

		QString id = jsonObj["id"].toString();
		Color diffuse = SceneConfigLoader::JsonArrayToColor(&jsonObj["diffuse"].toArray());
		Color ambient = SceneConfigLoader::JsonArrayToColor(&jsonObj["ambient"].toArray());
		float reflectionRatio = 0.;
		if (jsonObj.contains("reflectionRatio"))
			reflectionRatio = jsonObj["reflectionRatio"].toDouble();

		unsigned matId = mgr->CurrentScene->AddMaterial(new StandardMaterial(diffuse, ambient, reflectionRatio));

		//dodanie materia³u do s³ownika
		this->_MaterialMap.insert(id, matId);
	}
}
void SceneConfigLoader::ParseObjects(const QJsonValue *jsonObjs, GLMgr *mgr)
{
	for each (QJsonValue val in jsonObjs->toArray())
	{
		QJsonObject jsonObj = val.toObject();
		
		QByteArray bta = jsonObj["filePath"].toString().toUtf8();
		const char *filePath = bta.data();
		Vector3D position = SceneConfigLoader::JsonArrayToVector3D(&jsonObj["position"].toArray());
		//pobieranie materia³u z mapy ju¿ sparsowanych
		unsigned matId = this->_MaterialMap[jsonObj["materialId"].toString()];

		float scale = 1.;
		if (jsonObj.contains("scale"))
			scale = jsonObj["scale"].toDouble();

		//dodanie obiektu na scene
		mgr->CurrentScene->AddObject(new Mesh(filePath, &position, scale), matId);
	}
}
void SceneConfigLoader::ParseLights(const QJsonValue *jsonLights, GLMgr *mgr)
{
	QJsonObject jsonObj = jsonLights->toObject();

	QByteArray bta = jsonObj["meshFilePath"].toString().toUtf8();
	const char *lightMeshPath = bta.data();

	for each (QJsonValue val in jsonObj.value("light").toArray())
	{
		QJsonObject jsonObj = val.toObject();


		Vector3D position = SceneConfigLoader::JsonArrayToVector3D(&jsonObj["position"].toArray());
		Color diffuse = SceneConfigLoader::JsonArrayToColor(&jsonObj["diffuse"].toArray());
		Color ambient = SceneConfigLoader::JsonArrayToColor(&jsonObj["ambient"].toArray());

		//stworzenie materia³u
		unsigned lightMaterial = mgr->CurrentScene->AddMaterial(new StandardMaterial(diffuse));

		//dodanie obiektu na scenie
		mgr->CurrentScene->AddObject(new Mesh(lightMeshPath, &position), lightMaterial);

		//dodanie œwiat³a
		mgr->LightList.append(new PointLight(position, diffuse, ambient));
	}
}

#pragma endregion 

#pragma region Ctors

SceneConfigLoader::SceneConfigLoader(const char *sceneConfigFile) :_SceneConfigFilePath(sceneConfigFile)
{
}

#pragma endregion 
