#pragma once
#include "IMaterial.h"
#include "MetaTypes.h"
#include "PointLight.h"
class StandardMaterial:
	public IMaterial
{
public:
	Color DiffuseColor;
	Color AmbientColor;
	Color ColorSpecular;
	Color ColorEmissive;
	float ReflectionRatio;

	Color CalculateDiffuseColor(Color lightColor, Vector3D inVector, Vector3D normal)
	{
		Color result(1,1,1), diffResult;

		inVector.normalize();
		double diffRate = Vector3D::dotProduct(inVector, normal);
		if(diffRate < 0)
			diffResult.setRgb(0,0,0);
		else
		{
			diffResult = this->DiffuseColor * lightColor * diffRate;
		}
		
		result = result * diffResult;
		return result;
	}

	Color GetAmbientColor()
	{
		return this->AmbientColor;
	}

	StandardMaterial(Color diffColor, Color ambColor, float reflectionRatio) 
		:DiffuseColor(diffColor), AmbientColor(ambColor), ReflectionRatio(reflectionRatio)
	{
	}
	float *GetColorProperties(unsigned int *outCount)
	{
		*outCount = 3 * 4;
		float *tab = new float[*outCount];
		float *tmpCol;

		tmpCol = this->DiffuseColor.GetValue();
		memcpy(tab + (0 * 4), tmpCol, 4 * sizeof(float));
		//delete tmpCol;

		tmpCol = this->AmbientColor.GetValue();
		memcpy(tab + (1 * 4), tmpCol, 4 * sizeof(float));
		//delete tmpCol;

		tab[4 * 2] = ReflectionRatio;

		return tab;
	}
};

