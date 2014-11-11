#pragma once
#include "PointLight.h"
#include "MetaTypes.h"

///Klasa zawierajπca informacje o materiale.
class IMaterial
{
public:
	///Obliczanie lokalnego koloru dla danego punktu.
	virtual Color CalculateLocalColor(const PointLight *light, const Vector3D *point, const Vector3D *normal) = 0;

	///Pobieranie w≥aúciwoúci materia≥u.
	virtual float *GetColorProperties(unsigned int *outCount) = 0;

	///Informacja czy materia≥ jest przeüroczysty.
	virtual bool IsTransmissive() = 0;
	///Obliczanie koloru przeüroczystoúci.
	virtual Color CalculateTransmissionColor(const Color *baseColor, const Color *transmissiveColor) = 0;

	///Informacja czy materia≥ jest odbijajπcy.
	virtual bool IsReflective() = 0;
	///Obliczanie koloru odbicia.
	virtual Color CalculateReflectionColor(const Color *baseColor, const Color *reflectionColor) = 0;

	///Informacja czy materia≥ jest emisyjny.
	virtual bool IsEmissive() = 0;
	///Obliczanie koloru emisji.
	virtual Color CalculateEmissiveColor() = 0;
};

