#pragma once
#include "PointLight.h"
#include "MetaTypes.h"

class IMaterial
{
public:
	virtual Color CalculateLocalColor(const PointLight *light, const Vector3D *point, const Vector3D *normal) = 0;

	virtual float *GetColorProperties(unsigned int *outCount) = 0;

	virtual bool IsTransmissive() = 0;
	virtual Color CalculateTransmissionColor(const Color *baseColor, const Color *transmissiveColor) = 0;

	virtual bool IsReflective() = 0;
	virtual Color CalculateReflectionColor(const Color *baseColor, const Color *reflectionColor) = 0;

	virtual bool IsEmissive() = 0;
	virtual Color CalculateEmissiveColor() = 0;
};

