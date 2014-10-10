#include "stdafx.h"
#include "Scene.h"

HitResult Scene::HandleRay(Ray *ray)
{
	HitResult result;
	double minDistance = DBL_MAX;
	for each (ISceneObject *object in this->ObjectList)
	{
		//testing hit
		ObjectHitResult objResult = object->HitTest(ray);
		if (objResult.Result && objResult.MinDistance < minDistance)
		{
			//new result
			minDistance = objResult.MinDistance;
			result.Result = true;
			result.HitPoint = objResult.HitPoint;
			result.HitNormal = objResult.HitNormal;
			result.HitMaterial = (IMaterial*)object->Material;
		}
	}
	return result;
}