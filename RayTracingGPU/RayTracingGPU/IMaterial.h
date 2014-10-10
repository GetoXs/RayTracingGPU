#pragma once
class IMaterial
{
public:
	virtual Color CalculateDiffuseColor(Color lightColor, Vector3D inVector, Vector3D normal) = 0;
	virtual Color GetAmbientColor() = 0;
	virtual float *GetColorProperties(unsigned int *outCount) = 0;
};

