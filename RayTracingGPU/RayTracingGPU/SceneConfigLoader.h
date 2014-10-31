#pragma once
#include "MetaTypes.h"

class GLMgr;
class SceneConfigLoader
{
private:
	//mapa z informacjami o sparsowanych materiałach
	//wykorzystywana przy dodawaniu nowych obiektów do sceny
	QMap<QString, unsigned> _MaterialMap;

	//parsery poszczególnych elementów pliku
	void ParseRayTracer(const QJsonValue *jsonRT, GLMgr *mgr);
	void ParseCamera(const QJsonValue *jsonCam, GLMgr *mgr);
	void ParseMaterials(const QJsonValue *jsonMats, GLMgr *mgr);
	void ParseObjects(const QJsonValue *jsonObjs, GLMgr *mgr);
	void ParseLights(const QJsonValue *jsonLights, GLMgr *mgr);

public:
	QString SceneConfigFilePath;
	SceneConfigLoader(const char *sceneConfigFile);
	bool Parse(GLMgr *mgr);

#pragma region Static Inline
	static Vector3D JsonArrayToVector3D(QJsonArray *arr)
	{
		return Vector3D((*arr)[0].toDouble(), (*arr)[1].toDouble(), (*arr)[2].toDouble());
	}
	static Color JsonArrayToColor(QJsonArray *arr)
	{
		if (arr->count()>3)
			return Color((*arr)[0].toDouble(), (*arr)[1].toDouble(), (*arr)[2].toDouble(), (*arr)[3].toDouble());
		else
			return Color((*arr)[0].toDouble(), (*arr)[1].toDouble(), (*arr)[2].toDouble());

	}
#pragma endregion 
};

