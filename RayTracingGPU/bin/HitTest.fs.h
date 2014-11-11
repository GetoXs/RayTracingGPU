#version 330
struct SLightSource
{
	vec4 position;              //po³o¿enie Ÿród³a œwiat³a
	vec4 ambient;               //œwiat³o otoczenia
	vec4 diffuse;               //œwiat³o rozproszonego
};
uniform uint LightNum;
//tablica z Ÿród³ami œwiat³a
uniform SLightSource Light[10];

//poziom g³êbokoœci algorytmu
uniform uint RayTracerDepth;

//zmienna wejœciowa zawiera informacje o wspolrzednych pixela w przestrzeni widoku
in vec4 gl_FragCoord;

//zmienne wyjœciowe
//wyjœciowy kolor
layout(location = 0) out vec4 outFragColor;
//wyjœciowy bufor testowy
layout(location = 1) out vec3 outTest;

// tekstura buforowa z indeksami wspó³rzêdnych wierzcho³ka
uniform isamplerBuffer PositionIndexTexture;
// tekstura buforowa z danymi wspó³rzêdnych wierzcho³ków
uniform samplerBuffer PositionCordsTexture;

// tekstura buforowa z indeksami materialu
uniform isamplerBuffer MaterialIndexTexture;
// tekstura buforowa z wlasciwosciami materialu
uniform samplerBuffer MaterialPropertiesTexture;

//wektor okreslaj¹cy wielkoœæ okna renderingu
uniform vec4 Viewport;

//macierz, bêd¹ca odwrotn¹ macierz¹ projekcji oraz widoku. Wymagana przy obliczaniu pocz¹tkowego promienia, pozwala na zamianê wspó³rzêdnych okna na wspó³rzêdne œwiata
uniform mat4 ProjectionMatrixInverse;

struct SMaterial
{
	vec4 ambient;			//œwiat³o otoczenia
	vec4 diffuse;			//œwiat³o rozproszenia
	vec4 emissive;			//œwiat³o emisji
	float reflectionRatio;	//wspó³czynnik odbicia
};
//Funckja realizuj¹ca funkcjê generowania promieni pierwotnych
void GetRay2D(vec4 pix, inout vec3 rayOrigin, inout vec3 rayDir)
{
	vec4 near, far;

	//przekszta³cenie ze wspó³rzêdnych vp na ortho 2d (-1; 1)
	pix.x = (pix.x - Viewport[0]) / Viewport[2];
	pix.y = (pix.y - Viewport[1]) / Viewport[3];

	//ustawianie konca i pocz¹tku obliczanego wektora
	far = vec4(pix.xy, 1.0, 1.0);
	near = vec4(pix.xy, 0.0, 1.0);
	near.xyz *= 2.0;
	near.xyz -= 1.0;
	far.xyz *= 2.0;
	far.xyz -= 1.0;

	//przekszta³cenie ze wspó³rzêdnych ortho na perspektywiczne
	far = ProjectionMatrixInverse * far;
	near = ProjectionMatrixInverse * near;
	far /= far.w;
	near /= near.w;

	//obliczanie kierunku
	rayDir = normalize(far.xyz - near.xyz);
	rayOrigin = near.xyz;
}
//Liczba trójk¹tów na scenie
int GetTrianglesCount()
{
	return textureSize(PositionIndexTexture) / 3;
}
//Funckja pobieraj¹ca wspó³rzedne wierzcho³ków trójk¹ta
void GetTriangleCoords(int triangleIndex, inout vec3 v0, inout vec3 v1, inout vec3 v2)
{
	int i0 = texelFetch(PositionIndexTexture, 3 * triangleIndex + 0).x;
	int i1 = texelFetch(PositionIndexTexture, 3 * triangleIndex + 1).x;
	int i2 = texelFetch(PositionIndexTexture, 3 * triangleIndex + 2).x;
	v0 = texelFetch(PositionCordsTexture, i0).xyz;
	v1 = texelFetch(PositionCordsTexture, i1).xyz;
	v2 = texelFetch(PositionCordsTexture, i2).xyz;
}

//definicja formatu tesktury materia³u
#define OFFSET_DIFFUSE 0
#define OFFSET_AMBIENT 1
#define OFFSET_EMISSIVE 2
#define OFFSET_REFLECTION_RATIO 3

#define OFFSET_MATERIAL (4)

//Funckja pobieraj¹ca w³aœciwosci materia³u trójk¹ta
void GetMaterial(int triangleIndex, inout SMaterial outMaterial)
{
	int i0 = texelFetch(MaterialIndexTexture, triangleIndex + 0).x;

	outMaterial.diffuse = texelFetch(MaterialPropertiesTexture, i0 * OFFSET_MATERIAL + OFFSET_DIFFUSE);
	outMaterial.ambient = texelFetch(MaterialPropertiesTexture, i0 * OFFSET_MATERIAL + OFFSET_AMBIENT);
	outMaterial.emissive = texelFetch(MaterialPropertiesTexture, i0 * OFFSET_MATERIAL + OFFSET_EMISSIVE);
	outMaterial.reflectionRatio = texelFetch(MaterialPropertiesTexture, i0 * OFFSET_MATERIAL + OFFSET_REFLECTION_RATIO).x;
}
//G³ówna funkcja do testowania przeciêcia promienia (rayOrigin, rayDit) z trójk¹tem (v0, v1, v2)
bool HitTest(vec3 v0, vec3 v1, vec3 v2, vec3 rayOrigin, vec3 rayDir,
	inout float dist, inout vec3 hitPoint, inout bool isFront, inout vec3 normal)
{
	dist = 0.0;

	//wyliczenie krawêdzi
	vec3 e0, e1;
	e0 = v1 - v0;
	e1 = v2 - v0;

	//wyliczenie wektora normalnego do trojkata
	normal = normalize(cross(e0, e1));

	//wektor prostopad³y do plaszczyzny (kierunku promienia i krawêdzi)
	vec3 s1 = cross(rayDir, e1);

	//sprawdzamy czy istnieje przeciecie wektora z drug¹ krawêdzi¹
	float d = dot(e0, s1);
	if (d == 0.0)
		return false;

	isFront = d > 0;
	if (!isFront)
		return false;

	float invD = 1.0 / d;

	//obliczanie pierwszej wspó³rzednej barycentrycznej
	vec3 d1 = rayOrigin - v0;
	float bc0 = dot(d1, s1) * invD;
	if (bc0 < 0.0 || bc0 > 1.0)
		return false;

	//obliczanie drugiej wspó³rzednej barycentrycznej
	vec3 s2 = cross(d1, e0);
	float bc1 = dot(rayDir, s2) * invD;
	if (bc1 < 0.0 || (bc0 + bc1) > 1.0)
		return false;

	//kalkulacja dystansu do plaszczyzny
	dist = dot(e1, s2) * invD;

	//sprawdzenie czy trojkat jest po wlaœciwej stronie promienia
	if (dist <= 0)
		return false;

	//punkt przeciêcia prostej z p³aszczyzn¹
	hitPoint = rayOrigin + (rayDir * dist);

	return true;
}
//Obliczanie koloru otoczenia metod¹ Lamberta
vec4 CalculateLocalAmbientLight(int lightIndex, vec4 matAmbientColor)
{
	vec4 lightColorAmbient = Light[lightIndex].ambient;
	return lightColorAmbient * matAmbientColor;
}
//Obliczanie lokalnego koloru metod¹ Lamberta
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
//Obliczanie koloru metod¹ Lamberta
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
//Pobieranie wartoœci wspó³czynnika przeŸroczystoœci
float GetTransmissiveRatio(SMaterial material)
{
	return ((material.ambient.a + material.diffuse.a) / 2.0);
}
//Pobieranie flagi odbicia materia³u
bool IsEmissive(SMaterial material)
{
	return material.emissive.a > 0;
}
//Obliczanie koloru z uwzglêdnieniem koloru przeŸroczystoœci
vec4 CalculateTransmissionColor(vec4 baseColor, vec4 transmissiveColor, float transmissiveRatio)
{
	return baseColor * transmissiveRatio + transmissiveColor * (1.0 - transmissiveRatio);
}
//Obliczanie koloru z uwzglêdnieniem koloru odbicia
vec4 CalculateReflectionColor(vec4 baseColor, vec4 reflectionColor, float reflectionRatio)
{
	return baseColor * (1.0 - reflectionRatio) + reflectionColor * reflectionRatio;
}
//G³ówna metoda realizuj¹ca funkcjonalnoœæ œledzenia promienia (rayOrigin, rayDir)
//zwracajaca informacje nt. trafienia oraz efektów materia³u
bool RayTrace(int depth, vec3 rayOrigin, vec3 rayDir, inout vec4 outColor,
	inout bool isTransmissive, inout vec3 transmissiveOrigin, inout vec3 transmissiveDir, inout float transmissiveRatio,
	inout bool isReflective, inout vec3 reflectionOrigin, inout vec3 reflectionDir, inout float reflectionRatio)
{
	isReflective = false;
	isTransmissive = false;

	//pobranie liczby trójk¹tów
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

	//dla ka¿dego trójk¹ta
	for (int i = 0; i < count; i++)
	{
		//pobranie wspó³rzêdnych
		GetTriangleCoords(i, v0, v1, v2);
		//test przeciêcia
		if (HitTest(v0, v1, v2, rayOrigin, rayDir, tmpDist, tmpHitPoint, isFront, tmpHitNormal))
		{
			//sprawdzanie czy znaleziony po³o¿ony jest bli¿ej od poprzedniego
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
		//w przypadku nieznalezienia przeciêcia
		outColor = vec4(1, 1, 1, 1);
		return false;
	}

	//pobieranie materia³u trójk¹ta
	SMaterial material;
	GetMaterial(hitTriangle, material);

	//wstêpna kalkulacja koloru
	outColor = CalculateLambertColor(material, hitPoint, hitNormal);
	//wspó³czynnik odbicia
	reflectionRatio = material.reflectionRatio;

	//sprawdzenie warunku poziomu g³êbokoœci oraz czy dany obiekt nie wytwarza œwiat³a (nie jest Ÿród³em œwiat³a)
	if (depth <= int(RayTracerDepth) && !IsEmissive(material))
	{
		//sprawdzenie przeŸroczystoœci
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

//funkcje do obs³ugi kolejnych poziomów g³êbokoœci techniki œledzenia promieni
void RayTrace6(vec3 rayOrigin, vec3 rayDir, inout vec4 outColor)
{
	vec3 transmissiveOrigin, transmissiveDir, reflectionOrigin, reflectionDir;
	bool isTransmissive, isReflective;
	float transmissiveRatio, reflectionRatio;
	vec4 transmissiveColor, reflectionColor;
	//testowanie przeciêcia oraz obliczanie koloru
	RayTrace(6, rayOrigin, rayDir, outColor, isTransmissive, transmissiveOrigin, transmissiveDir, transmissiveRatio,
		isReflective, reflectionOrigin, reflectionDir, reflectionRatio);
}
void RayTrace5(vec3 rayOrigin, vec3 rayDir, inout vec4 outColor)
{
	vec3 transmissiveOrigin, transmissiveDir, reflectionOrigin, reflectionDir;
	bool isTransmissive, isReflective;
	float transmissiveRatio, reflectionRatio;
	vec4 transmissiveColor, reflectionColor;
	//testowanie przeciêcia oraz obliczanie koloru
	RayTrace(5, rayOrigin, rayDir, outColor, isTransmissive, transmissiveOrigin, transmissiveDir, transmissiveRatio,
		isReflective, reflectionOrigin, reflectionDir, reflectionRatio);
	if (isTransmissive)
	{	//funckjonalnoœc przeŸroczystoœci
		RayTrace6(transmissiveOrigin, transmissiveDir, transmissiveColor);
		outColor = CalculateTransmissionColor(outColor, transmissiveColor, transmissiveRatio);
	}
	if (isReflective)
	{	//funckjonalnoœc refleksji
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
	//testowanie przeciêcia oraz obliczanie koloru
	RayTrace(4, rayOrigin, rayDir, outColor, isTransmissive, transmissiveOrigin, transmissiveDir, transmissiveRatio,
		isReflective, reflectionOrigin, reflectionDir, reflectionRatio);
	if (isTransmissive)
	{	//funckjonalnoœc przeŸroczystoœci
		RayTrace5(transmissiveOrigin, transmissiveDir, transmissiveColor);
		outColor = CalculateTransmissionColor(outColor, transmissiveColor, transmissiveRatio);
	}
	if (isReflective)
	{	//funckjonalnoœc refleksji
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
	//testowanie przeciêcia oraz obliczanie koloru
	RayTrace(3, rayOrigin, rayDir, outColor, isTransmissive, transmissiveOrigin, transmissiveDir, transmissiveRatio,
		isReflective, reflectionOrigin, reflectionDir, reflectionRatio);
	if (isTransmissive)
	{	//funckjonalnoœc przeŸroczystoœci
		RayTrace4(transmissiveOrigin, transmissiveDir, transmissiveColor);
		outColor = CalculateTransmissionColor(outColor, transmissiveColor, transmissiveRatio);
	}
	if (isReflective)
	{	//funckjonalnoœc refleksji
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
	//testowanie przeciêcia oraz obliczanie koloru
	RayTrace(2, rayOrigin, rayDir, outColor, isTransmissive, transmissiveOrigin, transmissiveDir, transmissiveRatio,
		isReflective, reflectionOrigin, reflectionDir, reflectionRatio);
	if (isTransmissive)
	{	//funckjonalnoœc przeŸroczystoœci
		RayTrace3(transmissiveOrigin, transmissiveDir, transmissiveColor);
		outColor = CalculateTransmissionColor(outColor, transmissiveColor, transmissiveRatio);
	}
	if (isReflective)
	{	//funckjonalnoœc refleksji
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
	//testowanie przeciêcia oraz obliczanie koloru
	RayTrace(1, rayOrigin, rayDir, outColor, isTransmissive, transmissiveOrigin, transmissiveDir, transmissiveRatio,
		isReflective, reflectionOrigin, reflectionDir, reflectionRatio);
	if (isTransmissive)
	{	//funckjonalnoœc przeŸroczystoœci
		RayTrace2(transmissiveOrigin, transmissiveDir, transmissiveColor);
		outColor = CalculateTransmissionColor(outColor, transmissiveColor, transmissiveRatio);
	}
	if (isReflective)
	{	//funckjonalnoœc refleksji
		RayTrace2(reflectionOrigin, reflectionDir, reflectionColor);
		outColor = CalculateReflectionColor(outColor, reflectionColor, reflectionRatio);
	}
}

void main()
{
	vec3 ray, origin;
	vec4 result;

	//pobieranie promienia we wspó³rzêdnych modelu
	GetRay2D(vec4(gl_FragCoord.xy, 0.0, 1.0), origin, ray);
	outTest = vec3(0);

	//rozpoczêcie promienia
	RayTrace1(origin, ray, result);

	//przypisanie wyjsciowego koloru
	outFragColor = result;
	return;
}