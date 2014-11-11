#pragma once
#include "MetaTypes.h"
#include "HitResult.h"
#include "PointLight.h"
#include "qlist.h"

///Klasa s³u¿aca do cieniowania sceny.
class Shading
{
public:
	///Obliczanie koloru dla trafionego punktu.
	static Color CalculateColor(HitResult *hitResult, QList<PointLight*> *lightList);
};

