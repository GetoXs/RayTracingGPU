#pragma once
#include "MetaTypes.h"
#include "HitResult.h"
#include "PointLight.h"
#include "qlist.h"

class Shading
{
public:
	static Color CalculateColor(HitResult *hitResult, QList<PointLight*> *lightList);
};

