#include "stdafx.h"
#include "Scene.h"

HitResult Scene::HandleRay(const Ray *ray)
{
	HitResult result;
	double minDistance = DBL_MAX;
	bool hitResult;
	ObjectHitResult objResult;
	int i = 0;
	for each (ISceneObject *object in this->ObjectList)
	{
		//testing hit
		hitResult = object->HitTest(ray, &objResult);
		if (hitResult && objResult.MinDistance < minDistance)
		{
			//new result
			minDistance = objResult.MinDistance;
			result.Result = true;
			result.HitPoint = objResult.HitPoint;
			result.HitNormal = objResult.HitNormal;
			result.HitMaterial = (IMaterial*)object->Material;
		}
		i++;
	}
	return result;
}