#pragma once
#include "IMaterial.h"
#include "MetaTypes.h"
#include "PointLight.h"

///Klasa z informacjami o materiale.
class StandardMaterial:
	public IMaterial
{
public:
	///Kolor rozproszenia.
	Color DiffuseColor;
	///Kolor otoczenia.
	Color AmbientColor;
	///Kolor emisji.
	Color EmissiveColor;
	///Kolor odbicia.
	float ReflectionRatio;

#pragma region Kalkulacje
	///Obliczanie lokalnego koloru otoczenia.
	Color CalculateLocalAmbientColor(const PointLight *light)
	{
		return this->AmbientColor * light->LightAmbientColor;
	}
	
	///Obliczanie lokalnego koloru dla danego punktu.
	virtual Color CalculateLocalColor(const PointLight *light, const Vector3D *point, const Vector3D *normal)
	{
		//obliczenie wektora prowadzacego od swiatla do oswietlanego punktu
		Vector3D inVector = (light->Position - *point);
		inVector.normalize();

		//wyliczenie natê¿enia padania swiat³a
		double diffRate = Vector3D::dotProduct(inVector, *normal);
		if (diffRate < 0)
		{	//punkt nieoœwietlony => swiat³o otoczenia
			return this->CalculateLocalAmbientColor(light);
		}
		else
		{
			return this->CalculateLocalAmbientColor(light) +
				this->DiffuseColor * light->LightDiffuseColor * diffRate;
		}
	}
	///Obliczanie koloru przeŸroczystoœci.
	Color CalculateTransmissionColor(const Color *baseColor, const Color *transmissiveColor)
	{
		float transmissiveRatio = this->GetTransmissiveRatio();
		return *baseColor * transmissiveRatio + *transmissiveColor * (1.0 - transmissiveRatio);
	}
	///Obliczanie koloru odbicia.
	Color CalculateReflectionColor(const Color *baseColor, const Color *reflectionColor)
	{
		return *baseColor * (1.0 - this->ReflectionRatio) + *reflectionColor * this->ReflectionRatio;
	}
	///Obliczanie koloru emisji.
	Color CalculateEmissiveColor()
	{
		return this->EmissiveColor;
	}
#pragma endregion

#pragma region Gety
	float GetTransmissiveRatio()
	{
		return (this->AmbientColor.alphaF() + this->DiffuseColor.alphaF()) / 2.0;
	}

	bool IsTransmissive()
	{
		return this->GetTransmissiveRatio() < 1.f - DBL_EPSILON;
	}

	bool IsReflective()
	{
		return this->ReflectionRatio > DBL_EPSILON;
	}
	bool IsEmissive()
	{
		return this->EmissiveColor.alphaF() > DBL_EPSILON;
	}

	virtual float *GetColorProperties(unsigned int *outCount)
	{
		*outCount = 4 * 4;
		float *tab = new float[*outCount];
		float *tmpCol;

		tmpCol = this->DiffuseColor.GetValue();
		memcpy(tab + (0 * 4), tmpCol, 4 * sizeof(float));
		//delete tmpCol;

		tmpCol = this->AmbientColor.GetValue();
		memcpy(tab + (1 * 4), tmpCol, 4 * sizeof(float));

		tmpCol = this->EmissiveColor.GetValue();
		memcpy(tab + (2 * 4), tmpCol, 4 * sizeof(float));

		tab[3 * 4] = ReflectionRatio;

		return tab;
	}
#pragma endregion

#pragma region Ctors
	StandardMaterial(Color diffColor, Color ambColor, Color emissiveColor, float reflectionRatio)
		:DiffuseColor(diffColor), AmbientColor(ambColor), ReflectionRatio(reflectionRatio), EmissiveColor(emissiveColor)
	{
	}
	StandardMaterial(Color diffColor, Color ambColor, float reflectionRatio)
		: StandardMaterial(diffColor, ambColor, Color(0.f), reflectionRatio)
	{
	}
	StandardMaterial(Color emissiveColor)
		: StandardMaterial(Color(0.f), Color(0.f), emissiveColor, 0.f)
	{
	}
#pragma endregion
};

