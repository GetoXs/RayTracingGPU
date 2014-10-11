#include "stdafx.h"
#include "Shading.h"

Color Shading::CalculateColor(HitResult *hitResult, QList<PointLight*> *lightList)
{
	Color result(0,0,0,0);
	for each (PointLight *light in (*lightList))
	{
		result += hitResult->HitMaterial->CalculateLocalColor(light, &hitResult->HitPoint, &hitResult->HitNormal);
	}
	result.Clamp();
	return result;
}