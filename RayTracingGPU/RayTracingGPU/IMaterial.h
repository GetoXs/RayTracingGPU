#pragma once
#include "PointLight.h"
#include "MetaTypes.h"

///Klasa zawieraj�ca informacje o materiale.
class IMaterial
{
public:
	///Obliczanie lokalnego koloru dla danego punktu.
	virtual Color CalculateLocalColor(const PointLight *light, const Vector3D *point, const Vector3D *normal) = 0;

	///Pobieranie w�a�ciwo�ci materia�u.
	virtual float *GetColorProperties(unsigned int *outCount) = 0;

	///Informacja czy materia� jest prze�roczysty.
	virtual bool IsTransmissive() = 0;
	///Obliczanie koloru prze�roczysto�ci.
	virtual Color CalculateTransmissionColor(const Color *baseColor, const Color *transmissiveColor) = 0;

	///Informacja czy materia� jest odbijaj�cy.
	virtual bool IsReflective() = 0;
	///Obliczanie koloru odbicia.
	virtual Color CalculateReflectionColor(const Color *baseColor, const Color *reflectionColor) = 0;

	///Informacja czy materia� jest emisyjny.
	virtual bool IsEmissive() = 0;
	///Obliczanie koloru emisji.
	virtual Color CalculateEmissiveColor() = 0;
};

