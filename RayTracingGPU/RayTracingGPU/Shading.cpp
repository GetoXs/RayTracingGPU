#include "stdafx.h"
#include "Shading.h"



Color Shading::CalculateColor(HitResult *hitResult, QList<PointLight*> *lightList)
{
	Color result(0,0,0);

	if(hitResult->Result)
	{
		for each (PointLight *light in (*lightList))
		{
			Vector3D inVector = (light->Position - hitResult->HitPoint).normalized();
			result = result + hitResult->HitMaterial->CalculateDiffuseColor(light->LightDiffuseColor, inVector, hitResult->HitNormal);
		}
	}
	else
	{
		result = result + hitResult->HitMaterial->GetAmbientColor();
	}
	return result;
}