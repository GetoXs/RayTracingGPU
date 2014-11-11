#version 330
struct SLightSource
{
	vec4 position;              //po�o�enie �r�d�a �wiat�a
	vec4 ambient;               //�wiat�o otoczenia
	vec4 diffuse;               //�wiat�o rozproszonego
};
uniform uint LightNum;
//tablica z �r�d�ami �wiat�a
uniform SLightSource Light[10];

//poziom g��boko�ci algorytmu
uniform uint RayTracerDepth;

//zmienna wej�ciowa zawiera informacje o wspolrzednych pixela w przestrzeni widoku
in vec4 gl_FragCoord;

//zmienne wyj�ciowe
//wyj�ciowy kolor
layout(location = 0) out vec4 outFragColor;
//wyj�ciowy bufor testowy
layout(location = 1) out vec3 outTest;

// tekstura buforowa z indeksami wsp�rz�dnych wierzcho�ka
uniform isamplerBuffer PositionIndexTexture;
// tekstura buforowa z danymi wsp�rz�dnych wierzcho�k�w
uniform samplerBuffer PositionCordsTexture;

// tekstura buforowa z indeksami materialu
uniform isamplerBuffer MaterialIndexTexture;
// tekstura buforowa z wlasciwosciami materialu
uniform samplerBuffer MaterialPropertiesTexture;

//wektor okreslaj�cy wielko�� okna renderingu
uniform vec4 Viewport;

//macierz, b�d�ca odwrotn� macierz� projekcji oraz widoku. Wymagana przy obliczaniu pocz�tkowego promienia, pozwala na zamian� wsp�rz�dnych okna na wsp�rz�dne �wiata
uniform mat4 ProjectionMatrixInverse;

struct SMaterial
{
	vec4 ambient;			//�wiat�o otoczenia
	vec4 diffuse;			//�wiat�o rozproszenia
	vec4 emissive;			//�wiat�o emisji
	float reflectionRatio;	//wsp�czynnik odbicia
};
//Funckja realizuj�ca funkcj� generowania promieni pierwotnych
void GetRay2D(vec4 pix, inout vec3 rayOrigin, inout vec3 rayDir)
{
	vec4 near, far;

	//przekszta�cenie ze wsp�rz�dnych vp na ortho 2d (-1; 1)
	pix.x = (pix.x - Viewport[0]) / Viewport[2];
	pix.y = (pix.y - Viewport[1]) / Viewport[3];

	//ustawianie konca i pocz�tku obliczanego wektora
	far = vec4(pix.xy, 1.0, 1.0);
	near = vec4(pix.xy, 0.0, 1.0);
	near.xyz *= 2.0;
	near.xyz -= 1.0;
	far.xyz *= 2.0;
	far.xyz -= 1.0;

	//przekszta�cenie ze wsp�rz�dnych ortho na perspektywiczne
	far = ProjectionMatrixInverse * far;
	near = ProjectionMatrixInverse * near;
	far /= far.w;
	near /= near.w;

	//obliczanie kierunku
	rayDir = normalize(far.xyz - near.xyz);
	rayOrigin = near.xyz;
}
//Liczba tr�jk�t�w na scenie
int GetTrianglesCount()
{
	return textureSize(PositionIndexTexture) / 3;
}
//Funckja pobieraj�ca wsp�rzedne wierzcho�k�w tr�jk�ta
void GetTriangleCoords(int triangleIndex, inout vec3 v0, inout vec3 v1, inout vec3 v2)
{
	int i0 = texelFetch(PositionIndexTexture, 3 * triangleIndex + 0).x;
	int i1 = texelFetch(PositionIndexTexture, 3 * triangleIndex + 1).x;
	int i2 = texelFetch(PositionIndexTexture, 3 * triangleIndex + 2).x;
	v0 = texelFetch(PositionCordsTexture, i0).xyz;
	v1 = texelFetch(PositionCordsTexture, i1).xyz;
	v2 = texelFetch(PositionCordsTexture, i2).xyz;
}

//definicja formatu tesktury materia�u
#define OFFSET_DIFFUSE 0
#define OFFSET_AMBIENT 1
#define OFFSET_EMISSIVE 2
#define OFFSET_REFLECTION_RATIO 3

#define OFFSET_MATERIAL (4)

//Funckja pobieraj�ca w�a�ciwosci materia�u tr�jk�ta
void GetMaterial(int triangleIndex, inout SMaterial outMaterial)
{
	int i0 = texelFetch(MaterialIndexTexture, triangleIndex + 0).x;

	outMaterial.diffuse = texelFetch(MaterialPropertiesTexture, i0 * OFFSET_MATERIAL + OFFSET_DIFFUSE);
	outMaterial.ambient = texelFetch(MaterialPropertiesTexture, i0 * OFFSET_MATERIAL + OFFSET_AMBIENT);
	outMaterial.emissive = texelFetch(MaterialPropertiesTexture, i0 * OFFSET_MATERIAL + OFFSET_EMISSIVE);
	outMaterial.reflectionRatio = texelFetch(MaterialPropertiesTexture, i0 * OFFSET_MATERIAL + OFFSET_REFLECTION_RATIO).x;
}
//G��wna funkcja do testowania przeci�cia promienia (rayOrigin, rayDit) z tr�jk�tem (v0, v1, v2)
bool HitTest(vec3 v0, vec3 v1, vec3 v2, vec3 rayOrigin, vec3 rayDir,
	inout float dist, inout vec3 hitPoint, inout bool isFront, inout vec3 normal)
{
	dist = 0.0;

	//wyliczenie kraw�dzi
	vec3 e0, e1;
	e0 = v1 - v0;
	e1 = v2 - v0;

	//wyliczenie wektora normalnego do trojkata
	normal = normalize(cross(e0, e1));

	//wektor prostopad�y do plaszczyzny (kierunku promienia i kraw�dzi)
	vec3 s1 = cross(rayDir, e1);

	//sprawdzamy czy istnieje przeciecie wektora z drug� kraw�dzi�
	float d = dot(e0, s1);
	if (d == 0.0)
		return false;

	isFront = d > 0;
	if (!isFront)
		return false;

	float invD = 1.0 / d;

	//obliczanie pierwszej wsp�rzednej barycentrycznej
	vec3 d1 = rayOrigin - v0;
	float bc0 = dot(d1, s1) * invD;
	if (bc0 < 0.0 || bc0 > 1.0)
		return false;

	//obliczanie drugiej wsp�rzednej barycentrycznej
	vec3 s2 = cross(d1, e0);
	float bc1 = dot(rayDir, s2) * invD;
	if (bc1 < 0.0 || (bc0 + bc1) > 1.0)
		return false;

	//kalkulacja dystansu do plaszczyzny
	dist = dot(e1, s2) * invD;

	//sprawdzenie czy trojkat jest po wla�ciwej stronie promienia
	if (dist <= 0)
		return false;

	//punkt przeci�cia prostej z p�aszczyzn�
	hitPoint = rayOrigin + (rayDir * dist);

	return true;
}
//Obliczanie koloru otoczenia metod� Lamberta
vec4 CalculateLocalAmbientLight(int lightIndex, vec4 matAmbientColor)
{
	vec4 lightColorAmbient = Light[lightIndex].ambient;
	return lightColorAmbient * matAmbientColor;
}
//Obliczanie lokalnego koloru metod� Lamberta
vec4 CalculateLocalLambertColor(int lightIndex, SMaterial material, vec3 point, vec3 normal)
{
	vec3 lightPosition = Light[lightIndex].position.xyz;
	vec4 lightColorDiffuse = Light[lightIndex].diffuse;
	//wektor padania swiatla
	vec3 inVector = normalize(lightPosition - point);
	float diffuseRate = dot(inVector, normal);
	
	if (diffuseRate < 0)
		return CalculateLocalAmbientLight(lightIndex, material.ambient);
	else
	{
		return CalculateLocalAmbientLight(lightIndex, material.ambient) +
			lightColorDiffuse * material.diffuse * diffuseRate;
	}
}
//Obliczanie koloru metod� Lamberta
vec4 CalculateLambertColor(SMaterial material, vec3 point, vec3 normal)
{
	vec4 outColor = material.emissive;
	for (int i = 0; i < int(LightNum); i++)
	{
		outColor += CalculateLocalLambertColor(i, material, point, normal);
	}
	clamp(outColor, 0, 1);
	return outColor;
}
//Pobieranie warto�ci wsp�czynnika prze�roczysto�ci
float GetTransmissiveRatio(SMaterial material)
{
	return ((material.ambient.a + material.diffuse.a) / 2.0);
}
//Pobieranie flagi odbicia materia�u
bool IsEmissive(SMaterial material)
{
	return material.emissive.a > 0;
}
//Obliczanie koloru z uwzgl�dnieniem koloru prze�roczysto�ci
vec4 CalculateTransmissionColor(vec4 baseColor, vec4 transmissiveColor, float transmissiveRatio)
{
	return baseColor * transmissiveRatio + transmissiveColor * (1.0 - transmissiveRatio);
}
//Obliczanie koloru z uwzgl�dnieniem koloru odbicia
vec4 CalculateReflectionColor(vec4 baseColor, vec4 reflectionColor, float reflectionRatio)
{
	return baseColor * (1.0 - reflectionRatio) + reflectionColor * reflectionRatio;
}
//G��wna metoda realizuj�ca funkcjonalno�� �ledzenia promienia (rayOrigin, rayDir)
//zwracajaca informacje nt. trafienia oraz efekt�w materia�u
bool RayTrace(int depth, vec3 rayOrigin, vec3 rayDir, inout vec4 outColor,
	inout bool isTransmissive, inout vec3 transmissiveOrigin, inout vec3 transmissiveDir, inout float transmissiveRatio,
	inout bool isReflective, inout vec3 reflectionOrigin, inout vec3 reflectionDir, inout float reflectionRatio)
{
	isReflective = false;
	isTransmissive = false;

	//pobranie liczby tr�jk�t�w
	int count = GetTrianglesCount();
	vec3 v0, v1, v2;
	int hitTriangle = -1;
	int hitDistance;

	float dist = 0, tmpDist = 0;
	vec3 hitPoint = vec3(0), tmpHitPoint = vec3(0);
	vec3 hitNormal = vec3(0), tmpHitNormal = vec3(0);
	bool isFront = false;

	vec3 uvw;
	float frontFacing;

	//dla ka�dego tr�jk�ta
	for (int i = 0; i < count; i++)
	{
		//pobranie wsp�rz�dnych
		GetTriangleCoords(i, v0, v1, v2);
		//test przeci�cia
		if (HitTest(v0, v1, v2, rayOrigin, rayDir, tmpDist, tmpHitPoint, isFront, tmpHitNormal))
		{
			//sprawdzanie czy znaleziony po�o�ony jest bli�ej od poprzedniego
			if (isFront && (hitTriangle == -1 || tmpDist < dist))
			{
				hitPoint = tmpHitPoint;
				hitTriangle = i;
				dist = tmpDist;
				hitNormal = tmpHitNormal;
			}
		}

	}
	if (hitTriangle == -1)
	{
		//w przypadku nieznalezienia przeci�cia
		outColor = vec4(1, 1, 1, 1);
		return false;
	}

	//pobieranie materia�u tr�jk�ta
	SMaterial material;
	GetMaterial(hitTriangle, material);

	//wst�pna kalkulacja koloru
	outColor = CalculateLambertColor(material, hitPoint, hitNormal);
	//wsp�czynnik odbicia
	reflectionRatio = material.reflectionRatio;

	//sprawdzenie warunku poziomu g��boko�ci oraz czy dany obiekt nie wytwarza �wiat�a (nie jest �r�d�em �wiat�a)
	if (depth <= int(RayTracerDepth) && !IsEmissive(material))
	{
		//sprawdzenie prze�roczysto�ci
		transmissiveRatio = GetTransmissiveRatio(material);
		isTransmissive = transmissiveRatio < 1;
		if (isTransmissive)
		{	//ustawiamy promien przezroczystosci
			transmissiveDir = rayDir;
			//dodatkowe minimalne przesuniecie (likwiduje bledy kolejnego przeciecia)
			transmissiveOrigin = hitPoint + transmissiveDir*0.0001;
		}
		//sprawdzenie odbicia
		isReflective = reflectionRatio > 0;
		if (isReflective)
		{	//ustawiamy promien odbicia
			vec3 rayDirInv = -rayDir;
			float refDot = dot(hitNormal, rayDirInv);
			reflectionDir = hitNormal * refDot * 2 - rayDirInv;
			reflectionDir = normalize(reflectionDir);
			reflectionOrigin = hitPoint;
		}
	}
	return true;
}

//funkcje do obs�ugi kolejnych poziom�w g��boko�ci techniki �ledzenia promieni
void RayTrace6(vec3 rayOrigin, vec3 rayDir, inout vec4 outColor)
{
	vec3 transmissiveOrigin, transmissiveDir, reflectionOrigin, reflectionDir;
	bool isTransmissive, isReflective;
	float transmissiveRatio, reflectionRatio;
	vec4 transmissiveColor, reflectionColor;
	//testowanie przeci�cia oraz obliczanie koloru
	RayTrace(6, rayOrigin, rayDir, outColor, isTransmissive, transmissiveOrigin, transmissiveDir, transmissiveRatio,
		isReflective, reflectionOrigin, reflectionDir, reflectionRatio);
}
void RayTrace5(vec3 rayOrigin, vec3 rayDir, inout vec4 outColor)
{
	vec3 transmissiveOrigin, transmissiveDir, reflectionOrigin, reflectionDir;
	bool isTransmissive, isReflective;
	float transmissiveRatio, reflectionRatio;
	vec4 transmissiveColor, reflectionColor;
	//testowanie przeci�cia oraz obliczanie koloru
	RayTrace(5, rayOrigin, rayDir, outColor, isTransmissive, transmissiveOrigin, transmissiveDir, transmissiveRatio,
		isReflective, reflectionOrigin, reflectionDir, reflectionRatio);
	if (isTransmissive)
	{	//funckjonalno�c prze�roczysto�ci
		RayTrace6(transmissiveOrigin, transmissiveDir, transmissiveColor);
		outColor = CalculateTransmissionColor(outColor, transmissiveColor, transmissiveRatio);
	}
	if (isReflective)
	{	//funckjonalno�c refleksji
		RayTrace6(reflectionOrigin, reflectionDir, reflectionColor);
		outColor = CalculateReflectionColor(outColor, reflectionColor, reflectionRatio);
	}
}
void RayTrace4(vec3 rayOrigin, vec3 rayDir, inout vec4 outColor)
{
	vec3 transmissiveOrigin, transmissiveDir, reflectionOrigin, reflectionDir;
	bool isTransmissive, isReflective;
	float transmissiveRatio, reflectionRatio;
	vec4 transmissiveColor, reflectionColor;
	//testowanie przeci�cia oraz obliczanie koloru
	RayTrace(4, rayOrigin, rayDir, outColor, isTransmissive, transmissiveOrigin, transmissiveDir, transmissiveRatio,
		isReflective, reflectionOrigin, reflectionDir, reflectionRatio);
	if (isTransmissive)
	{	//funckjonalno�c prze�roczysto�ci
		RayTrace5(transmissiveOrigin, transmissiveDir, transmissiveColor);
		outColor = CalculateTransmissionColor(outColor, transmissiveColor, transmissiveRatio);
	}
	if (isReflective)
	{	//funckjonalno�c refleksji
		RayTrace5(reflectionOrigin, reflectionDir, reflectionColor);
		outColor = CalculateReflectionColor(outColor, reflectionColor, reflectionRatio);
	}
}
void RayTrace3(vec3 rayOrigin, vec3 rayDir, inout vec4 outColor)
{
	vec3 transmissiveOrigin, transmissiveDir, reflectionOrigin, reflectionDir;
	bool isTransmissive, isReflective;
	float transmissiveRatio, reflectionRatio;
	vec4 transmissiveColor, reflectionColor;
	//testowanie przeci�cia oraz obliczanie koloru
	RayTrace(3, rayOrigin, rayDir, outColor, isTransmissive, transmissiveOrigin, transmissiveDir, transmissiveRatio,
		isReflective, reflectionOrigin, reflectionDir, reflectionRatio);
	if (isTransmissive)
	{	//funckjonalno�c prze�roczysto�ci
		RayTrace4(transmissiveOrigin, transmissiveDir, transmissiveColor);
		outColor = CalculateTransmissionColor(outColor, transmissiveColor, transmissiveRatio);
	}
	if (isReflective)
	{	//funckjonalno�c refleksji
		RayTrace4(reflectionOrigin, reflectionDir, reflectionColor);
		outColor = CalculateReflectionColor(outColor, reflectionColor, reflectionRatio);
	}
}
void RayTrace2(vec3 rayOrigin, vec3 rayDir, inout vec4 outColor)
{
	vec3 transmissiveOrigin, transmissiveDir, reflectionOrigin, reflectionDir;
	bool isTransmissive, isReflective;
	float transmissiveRatio, reflectionRatio;
	vec4 transmissiveColor, reflectionColor;
	//testowanie przeci�cia oraz obliczanie koloru
	RayTrace(2, rayOrigin, rayDir, outColor, isTransmissive, transmissiveOrigin, transmissiveDir, transmissiveRatio,
		isReflective, reflectionOrigin, reflectionDir, reflectionRatio);
	if (isTransmissive)
	{	//funckjonalno�c prze�roczysto�ci
		RayTrace3(transmissiveOrigin, transmissiveDir, transmissiveColor);
		outColor = CalculateTransmissionColor(outColor, transmissiveColor, transmissiveRatio);
	}
	if (isReflective)
	{	//funckjonalno�c refleksji
		RayTrace3(reflectionOrigin, reflectionDir, reflectionColor);
		outColor = CalculateReflectionColor(outColor, reflectionColor, reflectionRatio);
	}
}
void RayTrace1(vec3 rayOrigin, vec3 rayDir, inout vec4 outColor)
{
	vec3 transmissiveOrigin, transmissiveDir, reflectionOrigin, reflectionDir;
	bool isTransmissive, isReflective;
	float transmissiveRatio, reflectionRatio;
	vec4 transmissiveColor, reflectionColor;
	//testowanie przeci�cia oraz obliczanie koloru
	RayTrace(1, rayOrigin, rayDir, outColor, isTransmissive, transmissiveOrigin, transmissiveDir, transmissiveRatio,
		isReflective, reflectionOrigin, reflectionDir, reflectionRatio);
	if (isTransmissive)
	{	//funckjonalno�c prze�roczysto�ci
		RayTrace2(transmissiveOrigin, transmissiveDir, transmissiveColor);
		outColor = CalculateTransmissionColor(outColor, transmissiveColor, transmissiveRatio);
	}
	if (isReflective)
	{	//funckjonalno�c refleksji
		RayTrace2(reflectionOrigin, reflectionDir, reflectionColor);
		outColor = CalculateReflectionColor(outColor, reflectionColor, reflectionRatio);
	}
}

void main()
{
	vec3 ray, origin;
	vec4 result;

	//pobieranie promienia we wsp�rz�dnych modelu
	GetRay2D(vec4(gl_FragCoord.xy, 0.0, 1.0), origin, ray);
	outTest = vec3(0);

	//rozpocz�cie promienia
	RayTrace1(origin, ray, result);

	//przypisanie wyjsciowego koloru
	outFragColor = result;
	return;
}