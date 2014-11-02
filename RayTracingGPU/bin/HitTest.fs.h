#version 330
struct SLightSource
{
	vec4 position;              //po�o�enie �r�d�a �wiat�a
	vec4 ambient;               //intensywno�� �wiat�a otoczenia
	vec4 diffuse;               //intensywno�� �wiat�a rozproszonego
};
uniform uint LightNum;
uniform SLightSource Light[10];

uniform uint RayTracerDepth;

//zawiera informacje o wspolrzednych pixela w przestrzeni widoku
in vec4 gl_FragCoord;
//varying vec3 pixel;

layout(location = 0) out vec4 outFragColor;
layout(location = 1) out vec3 outTest;

// tekstura buforowa z indeksami wsp�rz�dnych wierzcho�ka
uniform isamplerBuffer PositionIndexTexture;
// tekstura buforowa z danymi wsp�rz�dnych wierzcho�k�w
uniform samplerBuffer PositionCordsTexture;

// tekstura buforowa z danymi wektor�w normalnych
uniform samplerBuffer NormalTexture;
// tekstura buforowa z indeksami wsp�rz�dnych wierzcho�ka
uniform isamplerBuffer NormalIndexTexture;

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
	vec4 ambient;
	vec4 diffuse;
	vec4 emissive;
	float reflectionRatio;
};

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
	//rayOrigin = far;
	//return;

	//ustawianie zakresu -1, 1
	//far.xyz *= 2.0;
	//far.xyz -= 1.0;

	//ustawianie odleglosci
	//float eyeDist = -1.0;
	//near.z = eyeDist;
	//far.z = 0;

	//obliczanie kierunku
	rayDir = normalize(far.xyz - near.xyz);
	rayOrigin = near.xyz;
}
int GetTrianglesCount()
{
	return textureSize(PositionIndexTexture) / 3;
}
void GetTriangleCoords(int triangleIndex, inout vec3 v0, inout vec3 v1, inout vec3 v2)
{
	int i0 = texelFetch(PositionIndexTexture, 3 * triangleIndex + 0).x;
	int i1 = texelFetch(PositionIndexTexture, 3 * triangleIndex + 1).x;
	int i2 = texelFetch(PositionIndexTexture, 3 * triangleIndex + 2).x;
	v0 = texelFetch(PositionCordsTexture, i0).xyz;
	v1 = texelFetch(PositionCordsTexture, i1).xyz;
	v2 = texelFetch(PositionCordsTexture, i2).xyz;
}
void GetTriangleNormals(int triangleIndex, inout vec3 v0, inout vec3 v1, inout vec3 v2)
{
	int i0 = texelFetch(NormalIndexTexture, 3 * triangleIndex + 0).x;
	int i1 = texelFetch(NormalIndexTexture, 3 * triangleIndex + 1).x;
	int i2 = texelFetch(NormalIndexTexture, 3 * triangleIndex + 2).x;
	v0 = texelFetch(NormalTexture, i0).xyz;
	v1 = texelFetch(NormalTexture, i1).xyz;
	v2 = texelFetch(NormalTexture, i2).xyz;
}

#define OFFSET_DIFFUSE 0
#define OFFSET_AMBIENT 1
#define OFFSET_EMISSIVE 2
#define OFFSET_REFLECTION_RATIO 3

#define OFFSET_MATERIAL (4)

void GetMaterial(int triangleIndex, inout SMaterial outMaterial)
{
	int i0 = texelFetch(MaterialIndexTexture, triangleIndex + 0).x;

	outMaterial.diffuse = texelFetch(MaterialPropertiesTexture, i0 * OFFSET_MATERIAL + OFFSET_DIFFUSE);
	outMaterial.ambient = texelFetch(MaterialPropertiesTexture, i0 * OFFSET_MATERIAL + OFFSET_AMBIENT);
	outMaterial.emissive = texelFetch(MaterialPropertiesTexture, i0 * OFFSET_MATERIAL + OFFSET_EMISSIVE);
	outMaterial.reflectionRatio = texelFetch(MaterialPropertiesTexture, i0 * OFFSET_MATERIAL + OFFSET_REFLECTION_RATIO).x;
}
//bool FindIntersectionNT(vec3 v0, vec3 v1, vec3 v2, vec3 rayOrigin, vec3 rayDir,
//	inout float dist, inout vec3 hitPoint, inout bool isLine, inout bool isFront, inout vec3 normal)
//{
//	dist = 0.0;
//	isLine = false;
//
//	//krawedzie
//	vec3 e0, e1;
//	e0 = v1 - v0;
//	e1 = v2 - v0;
//	//wyliczenie wektora normalnego do trojkata
//	normal = normalize(cross(e0, e1));
//	float mdotn = (rayDir^normal);
//	float PlaneCoef = (normal^v0);	// Same coef for all three vertices.
//	float planarDist = (rayOrigin^normal) - PlaneCoef;
//
//	bool isFront = (mdotn <= 0.0);
//	if (frontFace) {
//		if (planarDist <= 0) {
//			return false;
//		}
//	}
//	else {
//		if (planarDist >= 0) {
//			return false;
//		}
//	}
//	*dist = -planarDist / mdotn;
//	vec3 q;
//	q = rayDir;
//	q *= *dist;
//	q += rayOrigin;						// Point of view line intersecting plane
//
//	// Compute barycentric coordinates
//	vec3 v = q;
//	v -= v0;
//
//
//}
bool HitTest(vec3 v0, vec3 v1, vec3 v2, vec3 rayOrigin, vec3 rayDir,
	inout float dist, inout vec3 hitPoint, inout bool isLine, inout bool isFront, inout vec3 normal)
{
	dist = 0.0;
	isLine = false;
	//hitPoint.xyz = v 0.0;

	//wyliczenie kraw�dzi
	vec3 e0, e1;
	e0 = v1 - v0;
	e1 = v2 - v0;

	//wyliczenie wektora normalnego do trojkata
	normal = normalize(cross(e0, e1));

	//wektor prostopad�y do plaszczyzny (kierunku promienia i kraw�dzi)
	vec3 s1 = cross(rayDir, e1);

	//sprawdzamy czy istnieje przeciecie wektora z drug� kraw�dzi�
	//float d = dot(s1, e0);
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

	if (bc0 <= 0.01 || bc0 >= 0.99 || bc1 <= 0.01 || bc1 >= 0.99)
		isLine = true;

	return true;
}

bool PickTriangle(const vec3 V0, const vec3 V1, const vec3 V2,
	const vec3 O, const vec3 D,
	inout vec3 P, inout vec3 uvw,
	inout float dist, inout float frontFacing)
{
	// obliczenie wektora normalnego p�aszczyzny
	vec3 N = cross(V1 - V0, V2 - V0);

	// obliczenie wsp�czynnika r�wnania prostej wyznaczonej
	// przez punkt pocz�tkowy i wektor kierunkowy; jest
	// to jednocze�nie odleg�o�� punktu przeci�cia od pocz�tku
	// promienia; jednocze�nie obliczamy stron� wielok�ta
	frontFacing = -dot(N, D);
	dist = (dot(N, O) - dot(V0, N)) / frontFacing;



	// punkt przeci�cia le�y przed pocz�tkiem promienia
	if (dist < 0.0) return false;

	// punkt przeci�cia prostej z p�aszczyzn�
	P = O + (D * dist);

	// obliczenie wsp�rz�dnych barycentrycznych punktu przeci�cia
	// prostej z p�aszczyzn�
	mat4 mt1 = mat4(1.0);
	mt1[0] = vec4(V0, 0);
	mt1[1] = vec4(V1, 0);
	mt1[2] = vec4(V2, 0);
	mat4 inv2 = inverse(mt1);

	mat3 mt = mat3(V0, V1, V2);
	mat3 inv = inverse(mt);
	uvw = inv * P;


	//if (inv2[0][0] == 0 && inv2[0][1] == 0 && inv2[0][2] == 0
	//	&& inv2[1][0] == 0 && inv2[1][1] == 0 && inv2[1][2] == 0
	//	&& inv2[2][0] > 0.99)
	//	discard;
	//if (mt[0] == vec3(-0.5, 0, 0)
	//	&& mt[1] == vec3(0.5, 0, 0)
	//	&& mt[2] == vec3(0, 1, 0))
	//	discard;
	//if (V0[0] == -0.5)
	//	discard;

	// test przeci�cia
	if (uvw[0] < 0.0 || uvw[0] > 1.0 || uvw[1] < 0.0 || uvw[0] + uvw[1] > 1.0) 
		return false;
	return true;
}

vec4 CalculateLocalAmbientLight(int lightIndex, vec4 matAmbientColor)
{
	vec4 lightColorAmbient = Light[lightIndex].ambient;
	return lightColorAmbient * matAmbientColor;
}
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
float GetTransmissiveRatio(SMaterial material)
{
	return ((material.ambient.a + material.diffuse.a) / 2.0);
}
bool IsEmissive(SMaterial material)
{
	return material.emissive.a > 0;
}
vec4 CalculateTransmissionColor(vec4 baseColor, vec4 transmissiveColor, float transmissiveRatio)
{
	return baseColor * transmissiveRatio + transmissiveColor * (1.0 - transmissiveRatio);
}
vec4 CalculateReflectionColor(vec4 baseColor, vec4 reflectionColor, float reflectionRatio)
{
	return baseColor * (1.0 - reflectionRatio) + reflectionColor * reflectionRatio;
}

bool RayTrace(int depth, vec3 rayOrigin, vec3 rayDir, inout vec4 outColor,
	inout bool isTransmissive, inout vec3 transmissiveOrigin, inout vec3 transmissiveDir, inout float transmissiveRatio,
	inout bool isReflective, inout vec3 reflectionOrigin, inout vec3 reflectionDir, inout float reflectionRatio)
{
	//rayOrigin = vec3(-0.25, 0.2, 1.5);
	isReflective = false;
	isTransmissive = false;


	int count = GetTrianglesCount();
	//count = 30000;
	//outTest = vec3(count);
	vec3 v0, v1, v2;
	int hitTriangle = -1;
	int hitDistance;

	float dist = 0, tmpDist = 0;
	vec3 hitPoint = vec3(0), tmpHitPoint = vec3(0);
	vec3 hitNormal = vec3(0), tmpHitNormal = vec3(0);
	bool isLine = false, isFront = false;

	vec3 uvw;
	float frontFacing;

	////pobranie tr�jk�ta
	//GetTriangleCoords(0, v0, v1, v2);
	////if (PickTriangle(vec3(-0.5, 0.0, 0.0), vec3(0.5, 0.0, 0.0), vec3(0.0, 1.0, 0.0), vec3(-1.0, 100.0, -1.0), vec3(0.0, 0.0, 1.0), tmpHitPoint, uvw, tmpDist, frontFacing))
	////if (PickTriangle(v0, v1, v2, rayOrigin.xyz, rayDir.xyz, tmpHitPoint, uvw, tmpDist, frontFacing))
	//if (HitTest(v0, v1, v2, rayOrigin.xyz, rayDir.xyz, dist, hitPoint, isLine))
	////if (v0.x == -.5 && v0.y == 0 && v0.z == 0)
	////if (v2.x == 0 && v2.y == 1 && v2.z == 0)
	////if (rayDir.y > 0)
	////if (rayOrigin.x < 0.41 && rayOrigin.x > 0.39 && rayOrigin.y > 0.49 && rayOrigin.y < 0.51)
	////if (pixel.y>400 && pixel.x>400)
	//if (rayOrigin.x < 0)
	////if (rayOrigin.z == 0 )
	////if (textureSize(PositionIndexTexture)==3)
	//if (HitTest(vec3(0, 0, 0.4), vec3(0.4, 0, 0.4), vec3(0.4, 0.4, 0.4), rayOrigin.xyz, rayDir.xyz, dist, hitPoint, isLine, isFront, tmpHitNormal))
	//if (HitTest(vec3(-0.2, 0, 0), vec3(0.2, 0, 0), vec3(0.2, 0, 0.4), rayOrigin.xyz, rayDir.xyz, dist, hitPoint, isLine, isFront, tmpHitNormal))
	//if (pixel.x>400 && pixel.z == 0)
	//if (count>40)
	//	outFragColor = vec4(0, 1, 0, 1);
	//else
	//	outFragColor = vec4(1, 1, 0, 1);
	//return;

	//if (rayOrigin.x<0.5)
	//	discard;


	for (int i = 0; i < count; i++)
	{
		isLine = false;
		GetTriangleCoords(i, v0, v1, v2);
		//if (PickTriangle(v0, v1, v2, rayOrigin.xyz, rayDir.xyz, tmpHitPoint, uvw, tmpDist, frontFacing))
		if (HitTest(v0, v1, v2, rayOrigin, rayDir, tmpDist, tmpHitPoint, isLine, isFront, tmpHitNormal))
		{
			//sprawdzanie czy po�o�ony bli�ej od poprzedniego
			if (isFront && (hitTriangle == -1 || tmpDist < dist))
			{
				hitPoint = tmpHitPoint;
				hitTriangle = i;
				dist = tmpDist;
				hitNormal = tmpHitNormal;

				//uvw = tmpUvw;
				//frontFacing = tmpFrontFacing;
			}
			if (isLine)
			{
				//outColor = vec4(0, 0, 1, 1);
				//return;
			}
		}

	}
	if (hitTriangle == -1)
	{
		outColor = vec4(1, 1, 1, 1);
		return false;
	}
	//if (hitPoint.z <-0.2)
	//{
	//	outColor = vec4(1, 0, 0, 1);
	//	return;
	//}

	//vec3 vn0, vn1, vn2;
	//GetTriangleNormals(hitTriangle, vn0, vn1, vn2);

	SMaterial material;
	GetMaterial(hitTriangle, material);

	outColor = CalculateLambertColor(material, hitPoint, hitNormal);
	//outColor = vec4(1, 0, 0, 1);
	reflectionRatio = material.reflectionRatio;

	if (depth <= int(RayTracerDepth) && !IsEmissive(material))
	{
		transmissiveRatio = GetTransmissiveRatio(material);
		isTransmissive = transmissiveRatio < 1;
		if (isTransmissive)
		{	//ustawiamy promien przezroczystosci
			transmissiveDir = rayDir;
			//dodatkowe minimalne przesuniecie (likwiduje bledy kolejnego przeciecia)
			transmissiveOrigin = hitPoint + transmissiveDir*0.0001;
		}
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
		outTest = vec3(1);
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

	//if (ray.z>0.0)
	//if (pixel.y>400)
	//if ((ProjectionMatrixInverse * vec4(pixel, 1.0)).x>0)
	//	outFragColor = vec4(0, 1, 0, 1);
	//else
	//	outFragColor = vec4(0, 1, 1, 1);
	//return;




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








	/*


	int count = GetTrianglesCount();
	vec3 v0, v1, v2;
	int hitTriangle = -1;
	int hitDistance;

	float dist = 0, tmpDist = 0;
	vec3 hitPoint = vec3(0), tmpHitPoint = vec3(0);
	vec3 hitNormal = vec3(0), tmpHitNormal = vec3(0);
	bool isLine = false, isFront = false;

	vec3 uvw;
	float frontFacing;

	////pobranie tr�jk�ta
	//GetTriangleCoords(0, v0, v1, v2);
	////if (PickTriangle(vec3(-0.5, 0.0, 0.0), vec3(0.5, 0.0, 0.0), vec3(0.0, 1.0, 0.0), vec3(-1.0, 100.0, -1.0), vec3(0.0, 0.0, 1.0), tmpHitPoint, uvw, tmpDist, frontFacing))
	////if (PickTriangle(v0, v1, v2, origin.xyz, ray.xyz, tmpHitPoint, uvw, tmpDist, frontFacing))
	//if (HitTest(v0, v1, v2, origin.xyz, ray.xyz, dist, hitPoint, isLine))
	////if (v0.x == -.5 && v0.y == 0 && v0.z == 0)
	////if (v2.x == 0 && v2.y == 1 && v2.z == 0)
	////if (ray.y > 0)
	////if (origin.x < 0.41 && origin.x > 0.39 && origin.y > 0.49 && origin.y < 0.51)
	////if (pixel.y>400 && pixel.x>400)
	//if (origin.x < 0)
	////if (origin.z == 0 )
	////if (textureSize(PositionIndexTexture)==3)
	//if (HitTest(vec3(0, 0, 0.4), vec3(0.4, 0, 0.4), vec3(0.4, 0.4, 0.4), origin.xyz, ray.xyz, dist, hitPoint, isLine, isFront, tmpHitNormal))
	//if (HitTest(vec3(-0.2, 0, 0), vec3(0.2, 0, 0), vec3(0.2, 0, 0.4), origin.xyz, ray.xyz, dist, hitPoint, isLine, isFront, tmpHitNormal))
	//if (pixel.x>400 && pixel.z == 0)
	//if (count>40)
	//	outFragColor = vec4(0, 1, 0, 1);
	//else
	//	outFragColor = vec4(1, 1, 0, 1);
	//return;

	//if (origin.x<0.5)
	//	discard;


	for (int i = 0; i < count; i++)
	{
		isLine = false;
		GetTriangleCoords(i, v0, v1, v2);
		//if (PickTriangle(v0, v1, v2, origin.xyz, ray.xyz, tmpHitPoint, uvw, tmpDist, frontFacing))
		if (HitTest(v0, v1, v2, origin.xyz, ray.xyz, tmpDist, tmpHitPoint, isLine, isFront, tmpHitNormal))
		{
			//sprawdzanie czy po�o�ony bli�ej od poprzedniego
			if (hitTriangle == -1 || tmpDist < dist)
			{
				hitPoint = tmpHitPoint;
				hitTriangle = i;
				dist = tmpDist;
				hitNormal = tmpHitNormal;

				//uvw = tmpUvw;
				//frontFacing = tmpFrontFacing;
			}
			if (isLine)
			{
				//outFragColor = vec4(0, 0, 1, 1);
				//return;
			}
			if (i == 13)
			{
				//outFragColor = vec4(0, 1, 1, 1);
				//return;
			}

		}

	}
	if (hitTriangle == -1)
	{
		outFragColor = vec4(1, 0, 0, 1);
		return;
	}

	vec3 vn0, vn1, vn2;
	GetTriangleNormals(hitTriangle, vn0, vn1, vn2);

	vec4 matDiffuse, matAmbient;
	float reflectionRatio;
	GetMaterial(hitTriangle, matDiffuse, matAmbient, reflectionRatio);

	result = CalculateLambertColor(matAmbient, matDiffuse, hitPoint, hitNormal);

	vec4 transColor;
	//if (IsTransmissive(matDiffuse, matAmbient))
	//{	//ustawiamy promien przezroczystosci
	//	//todo
	//	transColor = vec4(0);

	//	result += transColor;
	//}



	outFragColor = vec4(result.rgb, 1.0);
	//outFragColor = vec4(1, 1, 0, 1);


	//if (matDiffuse.xyz == vec3(1, 1, 0)
	//	&& matAmbient.xyz == vec3(0.2, 0.2, 0))
	//{
	//}else
	//	outFragColor = vec4(0, 1, 0, 1);

	//outFragColor = vec4(1, 1, 1, 1);
	*/
}