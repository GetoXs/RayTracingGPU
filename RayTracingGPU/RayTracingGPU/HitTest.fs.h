
#version 410
struct SLightSource
{
	vec4 position;              // po³o¿enie Ÿród³a œwiat³a punktowego/reflektora
	vec4 ambient;               // intensywnoœæ œwiat³a otoczenia; oznaczenie La
	vec4 diffuse;               // intensywnoœæ œwiat³a rozproszonego; oznaczenie Ld
};
uniform uint LightNum;
uniform SLightSource Light[10];

//zawiera informacje o wspolrzednych pixela w przestrzeni widoku
varying vec3 pixel;

layout(location = 0) out vec4 outFragColor;
layout(location = 1) out vec3 outTest;

// tekstura buforowa z indeksami wspó³rzêdnych wierzcho³ka
uniform isamplerBuffer PositionIndexTexture;
// tekstura buforowa z danymi wspó³rzêdnych wierzcho³ków
uniform samplerBuffer PositionCordsTexture;

// tekstura buforowa z danymi wektorów normalnych
uniform samplerBuffer NormalTexture;
// tekstura buforowa z indeksami wspó³rzêdnych wierzcho³ka
uniform isamplerBuffer NormalIndexTexture;

// tekstura buforowa z wlasciwosciami obiektu sceny 
uniform samplerBuffer SceneObjectPropertiesTexture;

// tekstura buforowa z indeksami materialu
uniform isamplerBuffer MaterialIndexTexture;
// tekstura buforowa z wlasciwosciami materialu
uniform samplerBuffer MaterialPropertiesTexture;


//uniform vec3 eyePosition;
uniform vec4 Viewport;

uniform mat4 ProjectionMatrixInverse;


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
#define OFFSET_RATIO 2

#define OFFSET_MATERIAL (3)

void GetMaterial(int triangleIndex, inout vec4 diffuse, inout vec4 ambient, inout float reflectionRatio)
{
	int i0 = texelFetch(MaterialIndexTexture, triangleIndex + 0).x;

	diffuse = texelFetch(MaterialPropertiesTexture, i0 * OFFSET_MATERIAL + OFFSET_DIFFUSE);
	ambient = texelFetch(MaterialPropertiesTexture, i0 * OFFSET_MATERIAL + OFFSET_AMBIENT);
	reflectionRatio = texelFetch(MaterialPropertiesTexture, i0 * OFFSET_MATERIAL + OFFSET_RATIO).x;
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

	//wyliczenie krawêdzi
	vec3 e0, e1;
	e0 = v1 - v0;
	e1 = v2 - v0;

	//wyliczenie wektora normalnego do trojkata
	normal = normalize(cross(e0, e1));

	//wektor prostopad³y do plaszczyzny (kierunku promienia i krawêdzi)
	vec3 s1 = cross(rayDir, e1);

	//sprawdzamy czy istnieje przeciecie wektora z drug¹ krawêdzi¹
	//float d = dot(s1, e0);
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
	if (v0 == vec3(0.1, 0, 0.4))
	{
		outTest = vec3(bc0,0,0);
	}
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

	if (bc0 <= 0.01 || bc0 >= 0.99 || bc1 <= 0.01 || bc1 >= 0.99)
		isLine = true;

	return true;
}

bool PickTriangle(const vec3 V0, const vec3 V1, const vec3 V2,
	const vec3 O, const vec3 D,
	inout vec3 P, inout vec3 uvw,
	inout float dist, inout float frontFacing)
{
	// obliczenie wektora normalnego p³aszczyzny
	vec3 N = cross(V1 - V0, V2 - V0);

	// obliczenie wspó³czynnika równania prostej wyznaczonej
	// przez punkt pocz¹tkowy i wektor kierunkowy; jest
	// to jednoczeœnie odleg³oœæ punktu przeciêcia od pocz¹tku
	// promienia; jednoczeœnie obliczamy stronê wielok¹ta
	frontFacing = -dot(N, D);
	dist = (dot(N, O) - dot(V0, N)) / frontFacing;



	// punkt przeciêcia le¿y przed pocz¹tkiem promienia
	if (dist < 0.0) return false;

	// punkt przeciêcia prostej z p³aszczyzn¹
	P = O + (D * dist);

	// obliczenie wspó³rzêdnych barycentrycznych punktu przeciêcia
	// prostej z p³aszczyzn¹
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

	// test przeciêcia
	if (uvw[0] < 0.0 || uvw[0] > 1.0 || uvw[1] < 0.0 || uvw[0] + uvw[1] > 1.0) 
		return false;
	return true;
}

vec4 CalculateLocalAmbientLight(int lightIndex, vec4 matAmbientColor)
{
	vec4 lightColorAmbient = Light[lightIndex].ambient;
	return lightColorAmbient * matAmbientColor;
}

vec4 CalculateLocalLambertColor(int lightIndex, vec4 matAmbientColor, vec4 matDiffuseColor, vec3 point, vec3 normal)
{
	vec3 lightPosition = Light[lightIndex].position.xyz;
	vec4 lightColorDiffuse = Light[lightIndex].diffuse;
	//wektor padania swiatla
	vec3 inVector = normalize(lightPosition - point);
	float diffuseRate = dot(inVector, normal);
	
	if (diffuseRate < 0)
		return CalculateLocalAmbientLight(lightIndex, matAmbientColor);
	else
	{
		return CalculateLocalAmbientLight(lightIndex, matAmbientColor) + 
			lightColorDiffuse * matDiffuseColor * diffuseRate;
	}
}
vec4 CalculateLambertColor(vec4 matAmbientColor, vec4 matDiffuseColor, vec3 point, vec3 normal)
{
	vec4 outColor = vec4(0, 0, 0, 0);
	for (int i = 0; i < LightNum; i++)
	{
		outColor += CalculateLocalLambertColor(i, matAmbientColor, matDiffuseColor, point, normal);
	}
	clamp(outColor, 0, 1);
	return outColor;
}

float GetTransmissiveRatio(vec4 matDiffuse, vec4 matAmbient)
{
	//todo: dodac specular
	return ((matDiffuse.a + matAmbient.a) / 2.0);

}
void RayTrace(int depth, vec3 rayOrigin, vec3 rayDir, inout vec4 outColor);

void RayTrace(int depth, vec3 rayOrigin, vec3 rayDir, inout vec4 outColor,
	inout bool isTransmissive, inout vec3 transmissiveOrigin, inout vec3 transmissiveDir, inout float transmissiveRatio,
	inout bool isReflective, inout vec3 reflectionOrigin, inout vec3 reflectionDir, inout float reflectionRatio)
{
	//rayOrigin = vec3(-0.25, 0.2, 1.5);


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

	////pobranie trójk¹ta
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
			//sprawdzanie czy po³o¿ony bli¿ej od poprzedniego
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
		outColor = vec4(0, 0, 0, 1);
		return;
	}
	//if (hitPoint.z <-0.2)
	//{
	//	outColor = vec4(1, 0, 0, 1);
	//	return;
	//}

	//vec3 vn0, vn1, vn2;
	//GetTriangleNormals(hitTriangle, vn0, vn1, vn2);

	vec4 matDiffuse, matAmbient;
	GetMaterial(hitTriangle, matDiffuse, matAmbient, reflectionRatio);

	outColor = CalculateLambertColor(matAmbient, matDiffuse, hitPoint, hitNormal);

	if (depth < 2)
	{
		transmissiveRatio = GetTransmissiveRatio(matDiffuse, matAmbient);
		isTransmissive = transmissiveRatio < 1;
		if (isTransmissive)
		{	//ustawiamy promien przezroczystosci
			transmissiveDir = rayDir;
			transmissiveOrigin = hitPoint + transmissiveDir*0.001;
		}
		isReflective = reflectionRatio > 0;
		if (isReflective)
		{	//ustawiamy promien odbicia
			//todo
			vec3 rayDirInv = -rayDir;
			float refDot = dot(hitNormal, rayDirInv);
			reflectionDir = hitNormal * refDot * 2 - rayDirInv;
			reflectionDir = normalize(reflectionDir);
			reflectionOrigin = hitPoint;

			//obliczanie koloru œwiat³a przeŸroczystego
			//outColor = outColor * outColor.a + transColor * (1.0 - transColor.a);
		}
		//TODO: reflection
	}


}

void RayTrace2(vec3 rayOrigin, vec3 rayDir, inout vec4 outColor)
{
	outColor = vec4(1, 0, 0, 1);
	vec3 transmissiveOrigin, transmissiveDir;
	vec3 reflectionOrigin, reflectionDir;
	bool isTransmissive, isReflective;
	float transmissiveRatio, reflectionRatio;

	RayTrace(2, rayOrigin, rayDir, outColor, isTransmissive, transmissiveOrigin, transmissiveDir, transmissiveRatio,
		isReflective, reflectionOrigin, reflectionDir, reflectionRatio);
}
void RayTrace1(vec3 rayOrigin, vec3 rayDir, inout vec4 outColor)
{
	vec3 transmissiveOrigin, transmissiveDir;
	vec3 reflectionOrigin, reflectionDir;
	bool isTransmissive, isReflective;
	float transmissiveRatio, reflectionRatio;
	vec4 transColor, reflectColor;

	RayTrace(1, rayOrigin, rayDir, outColor, isTransmissive, transmissiveOrigin, transmissiveDir, transmissiveRatio,
		isReflective, reflectionOrigin, reflectionDir, reflectionRatio);
	if (isTransmissive)
	{
		//outColor = vec4(1);
		RayTrace2(transmissiveOrigin, transmissiveDir, transColor);
		outColor = outColor * transmissiveRatio + transColor * (1.0 - transmissiveRatio);
		//outColor = transColor * (1.0 - outColor.a);
	}
	if (isReflective)
	{
		//outColor = vec4(1);
		//vec4 transColor;
		RayTrace2(reflectionOrigin, reflectionDir, reflectColor);
		outColor = outColor * (1.0 - reflectionRatio) + reflectColor * reflectionRatio;
		//outColor = transColor * (1.0 - outColor.a);
		//outColor = reflectionRatio * reflectColor;
	}
}

void main()
{

	outTest = vec3(0);
	//if (ray.z>0.0)
	//if (pixel.y>400)
	//if ((ProjectionMatrixInverse * vec4(pixel, 1.0)).x>0)
	//	outFragColor = vec4(0, 1, 0, 1);
	//else
	//	outFragColor = vec4(0, 1, 1, 1);
	//return;




	vec3 ray, origin;
	vec4 result;

	//pobieranie promienia we wspó³rzêdnych modelu
	GetRay2D(vec4(pixel.xy, 0.0, 1.0), origin, ray);
	//outTest = origin;

	//rozpoczêcie promienia
	RayTrace1(origin, ray, result);

	//przypisanie wyjsciowego koloru
	outFragColor = result;
	return;










































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

	////pobranie trójk¹ta
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
			//sprawdzanie czy po³o¿ony bli¿ej od poprzedniego
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
}