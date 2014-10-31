#include "stdafx.h"
#include "GLMgr.h"
#include "GLFrame.h"
#include "OpenGLHelper.h"
#include "Sphere.h"
#include "StandardMaterial.h"
#include "Shading.h"
#include "Mesh.h"

#include <stdio.h>
#include <direct.h>
#define GetCurrentDir _getcwd

const char* LIGHT_MESH_FILE = "\\..\\Models\\box_light.obj";

const char* MESH_FILE_BUNNY = "\\..\\Models\\bunny.obj";
const char* MESH_FILE_BUNNY_BIG = "\\..\\Models\\bunny.ply";
const char* MESH_FILE_SPHERE = "\\..\\Models\\Sphere1.obj";
const char* MESH_FILE1 = "C:\\Users\\Mateusz\\Desktop\\New folder\\assimp--3.0.1270-sdk\\test\\models\\OBJ\\box.obj";
const char* MESH_FILE2 = "C:\\Users\\Mateusz\\Desktop\\New folder\\assimp--3.0.1270-sdk\\test\\models\\OBJ\\box2.obj";

GLMgr *GLMgr::_instance = NULL;

//sta³e wspó³czynników transformacji (obs³uga klawiatura)
const float TRANSFORM_TRANSLATE_RATE = 0.08;
const float TRANSFORM_SCALE_RATE = 1.2;
const float TRANSFORM_ROTATE_ANGLE = 3;

//ustawienia okna oraz kamery
const int WINDOWS_SIZE_X = 800;
const int WINDOWS_SIZE_Y = 800;
const int PLANE_NEAR = 2;
const int PLANE_FAR = 7;

bool HitTest(Vector3D v0, Vector3D v1, Vector3D v2, Ray ray,
	float *dist, Vector3D *hitPoint, bool *isFront, Vector3D *normal)
{
	*dist = 0;

	//krawedzie
	Vector3D e0, e1;
	e0 = v1 - v0;
	e1 = v2 - v0; 
	
	*normal = Vector3D::crossProduct(e0, e1).normalized();

	//wektor prostopad³y do plaszczyzny
	Vector3D s1 = Vector3D::crossProduct(ray.Direction, e1);

	//sprawdzamy czy istnieje przeciecie wektora z drug¹ krawêdzi¹
	float d = Vector3D::dotProduct(s1, e0);
	if (d == 0.0)
		return false;

	*isFront = d > 0;

	float invD = 1 / d;

	//obliczanie pierwszej wspó³rzednej barycentrycznej
	Vector3D d1 = ray.Start - v0;
	float bc0 = Vector3D::dotProduct(d1, s1) * invD;
	if (bc0 < 0.0 || bc0 > 1.0)
		return false;

	//obliczanie drugiej wspó³rzednej barycentrycznej
	Vector3D s2 = Vector3D::crossProduct(d1, e0);
	float bc1 = Vector3D::dotProduct(ray.Direction, s2) * invD;
	if (bc1 < 0.0 || (bc0 + bc1) > 1.0)
		return false;

	*dist = Vector3D::dotProduct(e1, s2) * invD;

	//punkt przeciêcia prostej z p³aszczyzn¹
	*hitPoint = ray.Start + (ray.Direction * *dist);

	return true;
}


GLuint TestOutputBuffer;
GLuint TestOutputTexture;

void GLMgr::Init()
{
	GLenum numError = OpenGLHelper::CheckErrors();

#pragma region Viewport
	//ustawienia ekranu
	this->Viewport.setLeft(0);
	this->Viewport.setTop(0);
	this->Viewport.setRight(WINDOWS_SIZE_X);
	this->Viewport.setBottom(WINDOWS_SIZE_Y);
#pragma endregion

#pragma region Kamera
	float left = -1, right = 1;
	float top = -1, bottom = 1;
	//kamera
	if (this->Viewport.width() < this->Viewport.height() && this->Viewport.width() > 0)
	{
		this->Camera = new CameraPerspective(
			left*this->Viewport.height() / this->Viewport.width(), 
			right*this->Viewport.height() / this->Viewport.width(), 
			top, 
			bottom,
			PLANE_NEAR, PLANE_FAR);
	}
	else
	{
		this->Camera = new CameraPerspective(
			left, 
			right, 
			top*this->Viewport.width() / this->Viewport.height(), 
			bottom*this->Viewport.width() / this->Viewport.height(),
			PLANE_NEAR, PLANE_FAR);
	}
	
	//this->Camera = new CameraOrto(Vector3D(0, 0, -5), PointI(this->Viewport.width(), this->Viewport.height()),
	//	0.0, this->Viewport.width(), 0.0, this->Viewport.height(), -1.0, 1.0);
#pragma endregion

#pragma region Ustawienia macierzy dla raytracera
	this->Camera->ModelViewTranslate(0, 0, -(PLANE_NEAR + PLANE_FAR) / 2.f);
#pragma endregion

#pragma region Scena
	char cwd[MAX_PATH];
	GetCurrentDir(cwd, sizeof(cwd));
	char tmpPath[MAX_PATH];

	//tworzenie nowej sceny
	this->CurrentScene = new Scene();

	//parsowanie pliku konfiguracyjnego sceny
	bool result = this->SceneLoader->Parse(this);
	assert(result);

	/*
	//unsigned mat1 = this->CurrentScene->AddMaterial(new StandardMaterial(Color(1, 0, 0, 0.7), Color(0.6, 0, 0, 0.7), 0));
	//unsigned mat2 = this->CurrentScene->AddMaterial(new StandardMaterial(Color(0, 1, 0), Color(0., 0.5, 0.), 0.5));
	//unsigned mat3 = this->CurrentScene->AddMaterial(new StandardMaterial(Color(0, 1, 1), Color(0., 0.5, .5), 0));
	//unsigned matFullRef = this->CurrentScene->AddMaterial(new StandardMaterial(Color(0, 1, 0), Color(0., 0.5, 0.), 1));

	////2 szesciany
	//this->CurrentScene->AddObject(new Mesh(MESH_FILE1, &Vector3D(0.3, 0.3, -1.4)), mat2);
	//this->CurrentScene->AddObject(new Mesh(MESH_FILE1, &Vector3D(1.5, 1, 0),0.5), mat1);
	//this->Camera->ModelViewRotate(15, 0, 1, 0);
	//this->SetRayTracerDepth(3);

	//sprintf(tmpPath, "%s%s", cwd, MESH_FILE_BUNNY);
	//this->CurrentScene->AddObject(new Mesh(MESH_FILE1, &Vector3D(-.3, 0, -2), 2), matFullRef);
	//this->CurrentScene->AddObject(new Mesh(tmpPath, &Vector3D(0.1, -0.08, 0.02), 11), mat3);
	//this->Camera->ModelViewRotate(30, 0, 1, 0);

	//this->CurrentScene->AddObject(new Mesh(tmpPath, &Vector3D(0, -0.08, -0.02),11), mat2);
	//this->CurrentScene->AddObject(new Mesh(MESH_FILE1, &Vector3D(1.2, 0, 2), 0.3), mat1);

	//this->CurrentScene->AddObject(new Mesh(tmpPath, &Vector3D(0, -0.08, -0.02),11), mat2);
	//this->CurrentScene->AddObject(new Mesh(tmpPath, &Vector3D(0.12, -0.08, 0.1), 6), mat1);

	//sprintf(tmpPath, "%s%s", cwd, MESH_FILE_SPHERE);
	//this->CurrentScene->AddObject(new Mesh(MESH_FILE1, &Vector3D(0, 0, -0.65), 5), new StandardMaterial(Color(1, 1, 1), Color(0.5, 0.5, 0.5), 0.95));
	//this->CurrentScene->AddObject(new Mesh(tmpPath, &Vector3D(-1.5, 0, -0.4), 0.5), mat1);
	//this->CurrentScene->AddObject(new Mesh(tmpPath, &Vector3D(0, 0, 0), 1), mat2);
	//this->CurrentScene->AddObject(new Mesh(tmpPath, &Vector3D(1.5, 0, -0.4), 0.5), new StandardMaterial(Color(1, 1, 0), Color(0., 0.5, 0.), 0.8));
	//this->Camera->ModelViewRotate(30, 1, 0, 0);
	//this->SetRayTracerDepth(3);

	//sprintf(tmpPath, "%s%s", cwd, MESH_FILE_BUNNY_BIG);
	//this->CurrentScene->AddObject(new Mesh(tmpPath, &Vector3D(0, 0, 0), 9), new StandardMaterial(Color(0.9, 0.9, 0.9), Color(0.5, 0.5, 0.5), 0));
	//this->Camera->ModelViewRotate(-15, 0, 1, 0);

	//this->CurrentScene->AddObject(new Sphere(Vector3D(0, 0, 0), 20, new StandardMaterial(Color(1,1,0), Color(0.2,0.2,0))));
	//this->CurrentScene->AddObject(new Sphere(Vector3D(-40, 0, 0), 20, new StandardMaterial(Color(0,1,0), Color(0,0.2,0))));
	//this->CurrentScene->AddObject(new Sphere(Vector3D(40, 40, 0), 20, new StandardMaterial(Color(0,1,1), Color(0,0.2,0.2))));
	//this->CurrentScene->AddObject(new Sphere(Vector3D(80, -40, 0), 20, new StandardMaterial(Color(1,1,1), Color(0,0.2,0.2))));
	//this->CurrentScene->AddObject(new Sphere(Vector3D(120, 0, 0), 20, new StandardMaterial(Color(0,1,1), Color(0,0.2,0.2))));
	*/
#pragma endregion

#pragma region Light

	//sprintf(tmpPath, "%s%s", cwd, LIGHT_MESH_FILE);
	//Vector3D lightPosition;
	//unsigned lightMaterial;

	//lightPosition = Vector3D(-1., 1, 1);
	//lightMaterial = this->CurrentScene->AddMaterial(new StandardMaterial(Color(1, 1, 1)));
	//this->CurrentScene->AddObject(new Mesh(tmpPath, &lightPosition), lightMaterial);
	//this->LightList.append(new PointLight(lightPosition, Color(1.0, 1.0, 1.0), Color(.4, .4, .4)));

	//lightPosition = Vector3D(0., -1, 0);
	//lightMaterial = this->CurrentScene->AddMaterial(new StandardMaterial(Color(1., 0, 0)));
	//this->CurrentScene->AddObject(new Mesh(tmpPath, &lightPosition), lightMaterial);
	//this->LightList.append(new PointLight(lightPosition, Color(1.0, 0, 0), Color(.4, .0, .0)));
#pragma endregion

#pragma region Shader
	this->Shader_TestHit = OpenGLHelper::LoadShader("Main.vs.h", "HitTest.fs.h", NULL, 0, NULL);
	OpenGLHelper::CheckErrors();
	glUseProgram(0);
	OpenGLHelper::CheckErrors();
#pragma endregion
	
	//GLint indexArray[] = {	0, 1, 2,
	//						3, 4, 5};
	//GLfloat cordsArray[] = {-0.5, 0.0, 0.0, 
	//						0.5, 0.0, 0.0,
	//						0.0, 1.0, 0.0,
	//						.5, 0.0, 0.0,
	//						1.5, 0.0, 0.0,
	//						1., 1.0, 0.0 };
//	Assimp::Importer importer;
//	const aiScene* sc = importer.ReadFile(MESH_FILE1, aiProcess_GenNormals | aiProcess_Triangulate);
//	if (sc == NULL)
//		const char* erer = importer.GetErrorString();
//
//	const aiMesh* mesh = sc->mMeshes[0];
//	
//#pragma region Pobieranie indexow wierzcholkow oraz normalnych trójk¹tów
//	//konwersja struktury assimp na tablice
//	unsigned int *faceArray;
//	unsigned int *normalPrimitivesArray;
//	faceArray = new unsigned int[mesh->mNumFaces * 3];
//	normalPrimitivesArray = new unsigned int[mesh->mNumFaces];
//	unsigned int faceIndex = 0;
//	for (unsigned int t = 0; t < mesh->mNumFaces; ++t) 
//	{
//		const aiFace* face = &mesh->mFaces[t];
//		memcpy(&faceArray[faceIndex], face->mIndices, 3 * sizeof(unsigned int));
//		faceIndex += 3;
//		//normalPrimitivesArray[t] = mesh->
//	}
//#pragma endregion
//
//#pragma region Pobieranie indexow materia³ów oraz parametrów materia³u
//	unsigned int *materialIndexArray = new unsigned int[mesh->mNumVertices];
//	memset(materialIndexArray, 0, sizeof(unsigned int)*mesh->mNumVertices);
//
//	StandardMaterial m = StandardMaterial(Color(1, 1, 0), Color(0.2, 0.2, 0));
//	unsigned int materialBuffCount;
//	float *materialBuff = m.GetColorProperties(&materialBuffCount);
//	size_t materialBuffSize = materialBuffCount * sizeof(float);
//#pragma endregion
//	//OpenGLHelper::RegisterTexture(GL_TEXTURE0, faceArray, sizeof(unsigned int) * mesh->mNumFaces * 3, GL_R32I,
//	//	&TextureBuffer_PositionIndex, &Texture_PositionIndex);
//	//OpenGLHelper::RegisterTexture(GL_TEXTURE1, mesh->mVertices, sizeof(float) * 3 * mesh->mNumVertices, GL_RGB32F,
//	//	&TextureBuffer_PositionCords, &Texture_PositionCords);
//	//OpenGLHelper::RegisterTexture(GL_TEXTURE2, faceArray, sizeof(unsigned int) * mesh->mNumFaces * 3, GL_R32I,
//	//	&TextureBuffer_NormalIndex, &Texture_NormalIndex);
//	////normalnych tyle samo co wierzcholkow
//	//OpenGLHelper::RegisterTexture(GL_TEXTURE3, mesh->mNormals, sizeof(float) * 3 * mesh->mNumVertices, GL_RGB32F,
//	//	&TextureBuffer_NormalCords, &Texture_NormalCords);
//	//OpenGLHelper::RegisterTexture(GL_TEXTURE4, materialIndexArray, sizeof(unsigned int) * mesh->mNumVertices, GL_R32I,
//	//	&TextureBuffer_MaterialIndex, &Texture_MaterialIndex);
//	//OpenGLHelper::RegisterTexture(GL_TEXTURE5, materialBuff, materialBuffSize, GL_RGBA32F,
//	//	&TextureBuffer_MaterialProperties, &Texture_MaterialProperties);

#pragma region Rejestracja buforów oraz textur
	unsigned buffCount;
	float *tmpBufferF;
	unsigned *tmpBufferUI;

	glGenRenderbuffers(1, &this->RenderBuffer_Color);
	glBindRenderbuffer(GL_RENDERBUFFER, this->RenderBuffer_Color);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA32F, this->Viewport.width(), this->Viewport.height());
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	OpenGLHelper::CheckErrors();

	//z dodatkowym generowanym offsetem dla indexu
	tmpBufferUI = this->CurrentScene->GetPositionIndexData(&buffCount, true);
	OpenGLHelper::RegisterTexture(GL_TEXTURE0, tmpBufferUI, sizeof(unsigned int) * buffCount, GL_R32I,
		&TextureBuffer_PositionIndex, &Texture_PositionIndex);

	tmpBufferF = this->CurrentScene->GetPositionData(&buffCount);
	OpenGLHelper::RegisterTexture(GL_TEXTURE1, tmpBufferF, sizeof(float) * buffCount, GL_RGB32F,
		&TextureBuffer_PositionCords, &Texture_PositionCords);

	tmpBufferUI = this->CurrentScene->GetNormalIndexData(&buffCount);
	OpenGLHelper::RegisterTexture(GL_TEXTURE2, tmpBufferUI, sizeof(unsigned int) * buffCount, GL_R32I,
		&TextureBuffer_NormalIndex, &Texture_NormalIndex);

	tmpBufferF = this->CurrentScene->GetNormalData(&buffCount);
	OpenGLHelper::RegisterTexture(GL_TEXTURE3, tmpBufferF, sizeof(float) * buffCount, GL_RGB32F,
		&TextureBuffer_NormalCords, &Texture_NormalCords);

	tmpBufferUI = this->CurrentScene->GetMaterialIndexData(&buffCount);
	OpenGLHelper::RegisterTexture(GL_TEXTURE4, tmpBufferUI, sizeof(unsigned int) * buffCount, GL_R32I,
		&TextureBuffer_MaterialIndex, &Texture_MaterialIndex);

	tmpBufferF = this->CurrentScene->GetMaterialPropertiesData(&buffCount);
	OpenGLHelper::RegisterTexture(GL_TEXTURE5, tmpBufferF, buffCount * sizeof(float), GL_RGBA32F, 
		&TextureBuffer_MaterialProperties, &Texture_MaterialProperties);
	OpenGLHelper::CheckErrors();
#pragma endregion

#pragma region Generowanie testowego bufora textury
	//stworzenie buffora dla ramki testowej testowego w pamieci GL
	glGenBuffers(1, &TestOutputBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, TestOutputBuffer);

	glGenTextures(1, &TestOutputTexture);
	glBindTexture(GL_TEXTURE_2D, TestOutputTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, this->Viewport.width(), this->Viewport.height(),
		0, GL_RGB, GL_FLOAT, NULL);
#pragma endregion

//#pragma region PositionIndex
//	// tekstura z danymi indeksów wspó³rzêdnych wierzcho³ków
//	glGenBuffers(1, &TextureBuffer_PositionIndex);
//	glBindBuffer(GL_TEXTURE_BUFFER, TextureBuffer_PositionIndex);
//	//glBufferData(GL_TEXTURE_BUFFER, sizeof(indexArray), indexArray, GL_STATIC_DRAW);
//	glBufferData(GL_TEXTURE_BUFFER, sizeof(unsigned int) * mesh->mNumFaces * 3, faceArray, GL_STATIC_DRAW);
//	numError = OpenGLHelper::CheckErrors();
//
//	glActiveTexture(GL_TEXTURE0);
//
//	glGenTextures(1, &Texture_PositionIndex);
//	glBindTexture(GL_TEXTURE_BUFFER, Texture_PositionIndex);
//	glTexBuffer(GL_TEXTURE_BUFFER, GL_R32I, Texture_PositionIndex);
//	numError = OpenGLHelper::CheckErrors();
//#pragma endregion
//
//#pragma region PositionCords
//	// tekstura z danymi wspó³rzêdnych wierzcho³ków
//	glGenBuffers(1, &TextureBuffer_PositionCords);
//	glBindBuffer(GL_TEXTURE_BUFFER, TextureBuffer_PositionCords);
//	//glBufferData(GL_TEXTURE_BUFFER, sizeof(cordsArray), cordsArray, GL_STATIC_DRAW);
//	glBufferData(GL_TEXTURE_BUFFER, sizeof(float) * 3 * mesh->mNumVertices, mesh->mVertices, GL_STATIC_DRAW);
//	numError = OpenGLHelper::CheckErrors();
//
//	glActiveTexture(GL_TEXTURE1);
//
//	glGenTextures(1, &Texture_PositionCords);
//	glBindTexture(GL_TEXTURE_BUFFER, Texture_PositionCords);
//	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, Texture_PositionCords);
//	numError = OpenGLHelper::CheckErrors();
//#pragma endregion
//
//#pragma region MaterialIndex
//	// tekstura z danymi wspó³rzêdnych wierzcho³ków
//	glGenBuffers(1, &TextureBuffer_MaterialIndex);
//	glBindBuffer(GL_TEXTURE_BUFFER, TextureBuffer_MaterialIndex);
//	glBufferData(GL_TEXTURE_BUFFER, sizeof(unsigned int) * mesh->mNumVertices, materialIndexArray, GL_STATIC_DRAW);
//	numError = OpenGLHelper::CheckErrors();
//	delete[] materialIndexArray;
//
//	glActiveTexture(GL_TEXTURE4);
//
//	glGenTextures(1, &Texture_MaterialIndex);
//	glBindTexture(GL_TEXTURE_BUFFER, Texture_MaterialIndex);
//	glTexBuffer(GL_TEXTURE_BUFFER, GL_R32I, Texture_MaterialIndex);
//	numError = OpenGLHelper::CheckErrors();
//#pragma endregion
//
//#pragma region MaterialProperties
//	// tekstura z danymi wspó³rzêdnych wierzcho³ków
//	glGenBuffers(1, &TextureBuffer_MaterialProperties);
//	glBindBuffer(GL_TEXTURE_BUFFER, TextureBuffer_MaterialProperties);
//	glBufferData(GL_TEXTURE_BUFFER, sizeof(unsigned int) * mesh->mNumVertices, materialBuff, GL_STATIC_DRAW);
//	numError = OpenGLHelper::CheckErrors();
//	delete[] materialBuff;
//
//	glActiveTexture(GL_TEXTURE5);
//
//	glGenTextures(1, &Texture_MaterialProperties);
//	glBindTexture(GL_TEXTURE_BUFFER, Texture_MaterialProperties);
//	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, Texture_MaterialProperties);
//	numError = OpenGLHelper::CheckErrors();
//#pragma endregion

	glBindVertexArray(0);

#pragma region Generowanie obiektu bufora ramki
	glGenFramebuffers(1, &this->FrameBufferObject);
	OpenGLHelper::CheckErrors();
#pragma endregion

	/*
	1. napisanie funkcji do sprawdzania przeciêcia w FS
	2. Kolorowanie
	- Implementacja Frustrum i perspektywy po stronie FSa
	- Implementacja ruchu kamer¹ (transformy viewModelu)
	3. Wiele Ÿróde³ œwiat³a
	4. Testowanie przeciêæ rekurencyjne (bez sprawdzania przeszkód)
	5. Sprawdzanie przeszkód (cienie).
	6. Próba przerobienia tego na Compute Shadery
	- Dodanie mecirzy transformacji na ka¿dy z obiektow (informacja o indexie macierzy do tekstury, i sam index do osobnej textury)
	*/

	glClearColor(1.f, 0.f, 0.f, 1.0f);

#pragma region Ustawienia macierzy dla OpenGL
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//macierz projekcji ustawiona na 2d -> renderowanie perspektywiczne odbywa siê w shaderze
	Matrix4x4 ortho2D;
	ortho2D.ortho(this->Viewport.left(), this->Viewport.right(), this->Viewport.top(), this->Viewport.bottom(), -1., 1.);
	glLoadMatrixf(ortho2D.data());

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
#pragma endregion

	//start pêtli aplikacji
	glutMainLoop();
}

void GLMgr::RayTraceOnCPU(unsigned depth, const Ray *ray, Color *outColor)
{
	//testowanie trafienia
	HitResult hitResult = this->CurrentScene->HandleRay(ray);

	if (hitResult.Result)
	{
		//pobranie koloru trafionego punktu
		*outColor = Shading::CalculateColor(&hitResult, &this->LightList);
		//*outColor = Color(1, 0, 0);

		if (depth <= this->RayTracerDepth && !hitResult.HitMaterial->IsEmissive())
		{
			//wyliczenie przeŸroczystoœci
			if (hitResult.HitMaterial->IsTransmissive())
			{
				//wyznaczenie parametrów kolejnego promienia
				Vector3D transmissiveDir = ray->Direction;
				//dodatkowe minimalne przesuniecie (likwiduje bledy kolejnego przeciecia)
				Vector3D transmissiveOrigin = hitResult.HitPoint + transmissiveDir*0.0001;
				Color transmissiveColor = Color(1);
				//kolejny test promienia
				this->RayTraceOnCPU(depth + 1, &Ray(transmissiveOrigin, transmissiveDir), &transmissiveColor);
				//color blending
				*outColor = hitResult.HitMaterial->CalculateTransmissionColor(outColor, &transmissiveColor);
			}
			//wyliczenie odbicia
			if (hitResult.HitMaterial->IsReflective())
			{
				//wyznaczenie parametrów kolejnego promienia
				Vector3D rayDirectionInv = -ray->Direction;
				float reflectionDot = Vector3D::dotProduct(hitResult.HitNormal, rayDirectionInv);
				Vector3D reflectionDir = hitResult.HitNormal * reflectionDot * 2 - rayDirectionInv;
				Vector3D reflectionOrigin = hitResult.HitPoint;

				Color reflectionColor = Color(1);
				//kolejny test promienia
				this->RayTraceOnCPU(depth + 1, &Ray(reflectionOrigin, reflectionDir), &reflectionColor);
				//color blending
				*outColor = hitResult.HitMaterial->CalculateReflectionColor(outColor, &reflectionColor);
			}
		}
	}
	else
		*outColor = Color(1, 1, 1, 1);
}
void GLMgr::RenderSceneOnCPU()
{
	Ray *ray;
	Color pixelColor(0);

	OpenGLHelper::CheckErrors();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(0);
	OpenGLHelper::CheckErrors();

	glBegin(GL_POINTS);
	for (int y = 0; y < this->Viewport.width(); y++)
	{
		for (int x = 0; x < this->Viewport.height(); x++)
		{
			pixelColor = Color(1, 1, 1, 1);
			//pobranie promienia
			ray = &this->Camera->GetRay(PointI(x, y), this->Viewport);
			//Ray ray = this->Camera->GetRay(PointI(x, y));

			this->RayTraceOnCPU(1, ray, &pixelColor);

			//rysowanie pixela
			glColor3f(pixelColor.redF(), pixelColor.greenF(), pixelColor.blueF());
			glVertex2i(x, y);
		}
	}
	glEnd();
	OpenGLHelper::CheckErrors();

	glFlush();
	glutSwapBuffers();
	glutPostRedisplay();
	FpsObject.Update();
	FpsRaportObject.Update();
	OpenGLHelper::CheckErrors();
}
const bool IS_ADDITIONAL_FRAMEBUFFER = false;
void GLMgr::RenderSceneOnGPU()
{
	GLenum numError = OpenGLHelper::CheckErrors();

#pragma region Wypelnienie ramki
	if (IS_ADDITIONAL_FRAMEBUFFER)
	{
		//ustawienie tamki dla tekstury testowej
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->FrameBufferObject);
		//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		OpenGLHelper::CheckErrors();
		GLenum drawBuffer[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		//dowiazanie wyjsciowej ramki renderingu
		glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, this->RenderBuffer_Color);
		OpenGLHelper::CheckErrors();
		//dowiazanie wyjsciowej testury testowej
		glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, TestOutputTexture, 0);
		OpenGLHelper::CheckErrors();
		//ustawienie ramek do renderingu
		glDrawBuffers(sizeof(drawBuffer) / sizeof(GLenum), drawBuffer);
		OpenGLHelper::CheckErrors();
	}
	else
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	}
#pragma endregion

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(this->Shader_TestHit);
	numError = OpenGLHelper::CheckErrors();

#pragma region Setting Uniform

	/*
	Matrix4x4 modelViewMatrix;
	modelViewMatrix.setToIdentity();
	modelViewMatrix.translate(0, 0, -3);
	//modelViewMatrix.scale(2);
	//modelViewMatrix.rotate(90, 0, 1, 0);

	//Matrix4x4 modelViewProjectionMatrix = this->Camera->_ProjectionMatrix* modelViewMatrix;

	this->Camera->SetModelViewMatrix(&modelViewMatrix);
	const Matrix4x4 modelViewProjInverted = *this->Camera->GetModelViewProjectionMatrixInverted();
	Matrix4x4 modelViewInverted = modelViewMatrix.inverted();

	//wyliczanie promienia
	QVector4D vNear = QVector4D(800, 800, 0, 1.0);
	vNear.setX((vNear.x() - Viewport.left()) / Viewport.right());
	vNear.setY((vNear.y() - Viewport.top()) / Viewport.bottom());
	QVector4D vFar = QVector4D(vNear);
	vFar.setZ(1.0);
	vNear *= 2.0;
	vNear -= QVector4D(1.0, 1.0, 1.0, 1.0);
	vFar *= 2.0;
	vFar -= QVector4D(1.0, 1.0, 1.0, 1.0);
	QVector4D voNear = modelViewProjInverted * vNear;
	QVector4D voFar = modelViewProjInverted * vFar;
	voNear /= voNear.w();
	voFar /= voFar.w();

	QVector3D boN = QVector3D(voNear);
	QVector3D boF = QVector3D(voFar);
	QVector3D voRay = boF - boN;
	voRay.normalize();
	*/

	//ustawienie viewportu do kalkulacji na wspó³rzedne orto
	glUniform4f(glGetUniformLocation(Shader_TestHit, "Viewport"),
		this->Viewport.left(), this->Viewport.top(), this->Viewport.right(), this->Viewport.bottom());
	//ustawienie macierzy MVP do kalkulacji na wspólrzedne modelu
	glUniformMatrix4fv(OpenGLHelper::GetUniformLocation(Shader_TestHit, "ProjectionMatrixInverse"), 1, GL_FALSE,
		this->Camera->GetModelViewProjectionMatrixInverted()->data());
	//ustawienie tekstur
	glUniform1i(OpenGLHelper::GetUniformLocation(Shader_TestHit, "PositionIndexTexture"), 0);
	glUniform1i(OpenGLHelper::GetUniformLocation(Shader_TestHit, "PositionCordsTexture"), 1);
	glUniform1i(OpenGLHelper::GetUniformLocation(Shader_TestHit, "NormalTexture"), 2);
	glUniform1i(OpenGLHelper::GetUniformLocation(Shader_TestHit, "NormalIndexTexture"), 3);
	glUniform1i(OpenGLHelper::GetUniformLocation(Shader_TestHit, "MaterialIndexTexture"), 4);
	glUniform1i(OpenGLHelper::GetUniformLocation(Shader_TestHit, "MaterialPropertiesTexture"), 5);
	//ustawienie wlasciwosci swiatla
	OpenGLHelper::UpdateLightUniformList("Light", &this->LightList, Shader_TestHit, NULL);
	numError = OpenGLHelper::CheckErrors();
	//ustawienie glebokosci renderowania
	glUniform1ui(OpenGLHelper::GetUniformLocation(Shader_TestHit, "RayTracerDepth"), this->RayTracerDepth);
#pragma endregion

#pragma region Draw
	//Rysowanie prostok¹ta aby pokryæ ca³y ekran
	glBegin(GL_QUADS);
	glVertex3f(this->Viewport.left(), this->Viewport.bottom(), 0);
	glVertex3f(this->Viewport.right(), this->Viewport.bottom(), 0);
	glVertex3f(this->Viewport.right(), this->Viewport.top(), 0);
	glVertex3f(this->Viewport.left(), this->Viewport.top(), 0);
	glEnd();
	numError = OpenGLHelper::CheckErrors();
#pragma endregion

#pragma region Testy
	/*
	for (int y = 0; y < this->Viewport.width(); y++)
	{
		for (int x = 0; x < this->Viewport.height(); x++)
		{
			Color pixelColor;

			//1. Wygenerowañ promieñ dla aktualnego piksela
			Ray ray = this->Camera->GetRay(PointI(x, y), this->Viewport);
			//Ray ray = this->Camera->GetRay(PointI(x, y));
			Ray ray1(Vector3D(0.1, 0.1, -2.0), Vector3D(0.0, 0.0, 1.0));

			float dist = -1;
			Vector3D point(-1, -1, -1), normal;
			bool isFront;
			bool test = HitTest(Vector3D(-0.5, 0.0, 0.0), Vector3D(0.5, 0.0, 0.0), Vector3D(0.0, 1.0, 0.0),
				ray1, &dist, &point, &isFront, &normal);
			Vector3D uvw;
			float frontFacing = -1;
			//bool tt = PickTriangle(Vector3D(-0.5, 0.0, 0.0), Vector3D(0.5, 0.0, 0.0), Vector3D(0.0, 1.0, 0.0), ray1.Start, ray1.Direction,
			//	&point, &uvw, &dist, &frontFacing);

			//2. Dla ka¿dego obiektu na scenie
			// 1. Sprawdziæ czy promieñ przecina obiekt
			// 2. Jeœli przecina, sprardziæ czy odleg³oœc do kamery jest mniejsza od zapisanej
			// 3. Jeœli tak to podmieniæ i obliczac dalej
			//3. Jeœli zaleniono jakiœ obiekt na drodze promienia to sprawdziæ czy odbity promieñ dociera do Ÿróde³ œwiat³a
			HitResult hitResult = this->CurrentScene->HandleRay(&ray);



			if (hitResult.Result)
			{
				pixelColor = Shading::CalculateColor(&hitResult, &this->LightList);
				//pixelColor = Color(1,1,1);
			}
			//4. Jeœli dociera - wyliczyæ natê¿enie koloru w danym punkcie
			//5. Jeœli nie dociera - wyliczyæ natê¿enie w cieniu

			float red = pixelColor.redF();
			float green = pixelColor.greenF();
			float blue = pixelColor.blueF();
			//glColor3f(pixelColor.redF(), pixelColor.greenF(), pixelColor.blueF());
			//glBegin(GL_POINTS);
			//glVertex2i(x, y);
			//glEnd();
		}
	}
	*/
	numError = OpenGLHelper::CheckErrors();
#pragma endregion

#pragma region Przekopiowanie ramki na ekran
	if (IS_ADDITIONAL_FRAMEBUFFER)
	{
		//ustawienie bufora do odczytu
		glBindFramebuffer(GL_READ_FRAMEBUFFER, this->FrameBufferObject);
		OpenGLHelper::CheckErrors();
		glReadBuffer(GL_COLOR_ATTACHMENT0);
		OpenGLHelper::CheckErrors();
		//powrot do standardowej ramki
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		OpenGLHelper::CheckErrors();
		//przepisanie ramek na ca³¹ wielkoœc okna
		glBlitFramebuffer(0, 0, this->Viewport.width(), this->Viewport.height(),
			0, 0, this->Viewport.width(), this->Viewport.height(),
			GL_COLOR_BUFFER_BIT, GL_LINEAR);
		OpenGLHelper::CheckErrors();
	}
#pragma endregion
	glutSwapBuffers();
	OpenGLHelper::CheckErrors();
	glutPostRedisplay();
	FpsObject.Update();
}
void GLMgr::Raport()
{
	//dopisywanie kolejnych wierszy do pliku (format csv), wraz z nazw¹ pliku obrazka
	//zapisywanie obrazka z dat¹
	QFileInfo fi = (this->SceneLoader->SceneConfigFilePath);
	QString configFilename = fi.baseName();
	QString raportFilename = QString("Raport ")+configFilename + ".csv";
	QFile raportFile(raportFilename);
	bool fileNotExists = false;
	if (!raportFile.exists())
		fileNotExists = true;
	if (raportFile.open(QFile::Append | QFile::Text))
	{
		QTextStream txt(&raportFile);
		txt.setCodec("UTF-8");
		char delimiter = ';';
		if (fileNotExists)
		{
			txt << "Data pomiaru" << delimiter << "Tryb renderowania" << delimiter << "Liczba unikalnych wierzcholkow" << delimiter << "Liczba trojkatow" << delimiter << "Liczba swiatel" << delimiter << "Glebokosc RT" << delimiter << "Liczba klatek" << delimiter << "Czas testu" << delimiter << "Plik z obrazem" << endl;
		}

		QString mode = this->IsGPUMode() ? "GPU" : "CPU";
		QString imageExt = ".png";
		QDateTime currentDT = QDateTime::currentDateTime();
		QString displayFileName = currentDT.toString("yyyyMMdd-HHmmss") + "-RaportImage-" + mode + "-" + QString::number(this->RayTracerDepth) + imageExt;

		//Data pomiaru    |    Tryb    |    Liczba unikalnych wierzcho³ków    |    Liczba trójk¹tów    |    Liczba œwiate³    |    G³êbokoœc RT    |    Liczba klatek    |    Czas testu    |    Plik z obrazem
		txt << currentDT.toString(Qt::DateFormat::ISODate) << delimiter << mode << delimiter << this->CurrentScene->GetPositionCount() << delimiter << this->CurrentScene->GetTriangleCount() << delimiter << this->LightList.count()
			<< delimiter << this->RayTracerDepth << delimiter << this->FrameCounter << delimiter << this->IntervalRaport.value() << delimiter << displayFileName << endl;

		raportFile.close();
		
		this->SaveDisplayToFile("RaportImages\\" + displayFileName.toLocal8Bit());
	}
}
void GLMgr::SaveDisplayToFile(const char *filename)
{
	//inicjalizacja bufora
	unsigned length = this->Viewport.width() * this->Viewport.height() * 4;
	uchar *pixels = new uchar[length];
	memset(pixels, 0x0, length * sizeof(uchar));
	//kopiowanie danych
	glReadPixels(0, 0, this->Viewport.width(), this->Viewport.height(), GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	//zapisywanie do pliku
	QImage image = QImage(pixels, this->Viewport.width(), this->Viewport.height(), QImage::Format_ARGB32);
	image = image.rgbSwapped();
	//odwrocenie osi
	image = image.mirrored(false, true);
	
	//tworzenie katalogu
	QFileInfo fi(filename);
	QDir d = fi.dir();
	bool res = QDir().mkpath(d.path());

	res = image.save(filename);

	delete[] pixels;
	OpenGLHelper::CheckErrors();
}

void RenderScene(void)
{
	//aktualizacja licznika
	GLMgr::I()->UpdateCounter();

	//rysowanie sceny w³aœciwej
	if (GLMgr::I()->IsGPUMode())
		GLMgr::I()->RenderSceneOnGPU();
	else
		GLMgr::I()->RenderSceneOnCPU();
}
#pragma region Keyboard
void KeyboardFunc(unsigned char key, int x, int y)
{
	switch (key) {
	case VK_DELETE:	//klawisz DELETE
	case 0x7F:
		GLMgr::I()->ResetCounter();
		break;
	case ' ':
		GLMgr::I()->SwitchMode();
		break;
	case 'a':
		GLMgr::I()->GetCamera()->ModelViewTranslate(TRANSFORM_TRANSLATE_RATE, 0.f, 0.f);
		break;
	case 'd':
		GLMgr::I()->GetCamera()->ModelViewTranslate(-TRANSFORM_TRANSLATE_RATE, 0.f, 0.f);
		break;
	case 'w':
		GLMgr::I()->GetCamera()->ModelViewTranslate(0.f, 0.f, TRANSFORM_TRANSLATE_RATE);
		break;
	case 's':
		GLMgr::I()->GetCamera()->ModelViewTranslate(0.f, 0.f, -TRANSFORM_TRANSLATE_RATE);
		break;
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
		GLMgr::I()->SetRayTracerDepth(key-'0');
		break;
	case '+':
		GLMgr::I()->GetCamera()->ModelViewScale(TRANSFORM_SCALE_RATE);
		break;
	case '-':
		GLMgr::I()->GetCamera()->ModelViewScale(1/TRANSFORM_SCALE_RATE);
		break;
		
	}
}
void KeyboardSpecialFunc(int key, int x, int y)
{
	switch (key) {
	case GLUT_KEY_PAGE_UP:
		GLMgr::I()->GetCamera()->ModelViewTranslate(0.f, -TRANSFORM_TRANSLATE_RATE, 0.f);
		break;
	case GLUT_KEY_PAGE_DOWN:
		GLMgr::I()->GetCamera()->ModelViewTranslate(0.f, TRANSFORM_TRANSLATE_RATE, 0.f);
		break;
	case GLUT_KEY_LEFT:
		GLMgr::I()->GetCamera()->ModelViewRotate(TRANSFORM_ROTATE_ANGLE, 0.f, 1.f, 0.f);
		break;
	case GLUT_KEY_RIGHT:
		GLMgr::I()->GetCamera()->ModelViewRotate(-TRANSFORM_ROTATE_ANGLE, 0.f, 1.f, 0.f);
		break;
	case GLUT_KEY_DOWN:
		GLMgr::I()->GetCamera()->ModelViewRotate(-TRANSFORM_ROTATE_ANGLE, 1.f, 0.f, 0.f);
		break;
	case GLUT_KEY_UP:
		GLMgr::I()->GetCamera()->ModelViewRotate(TRANSFORM_ROTATE_ANGLE, 1.f, 0.f, 0.f);
		break;
	case GLUT_KEY_F1:	//tryb GPU
		GLMgr::I()->SetGPUMode(true);
		break;
	case GLUT_KEY_F2:	//tryb CPU
		GLMgr::I()->SetGPUMode(false);
		break;
	case GLUT_KEY_F11:	//raportowanie do pliku
		GLMgr::I()->Raport();
		break;
	case GLUT_KEY_F12:	//zapis okna do pliku
		GLMgr::I()->SaveDisplayToFile("test.png");
		break;
	}
}
#pragma endregion

void GLMgr::ResetCounter()
{
	this->FrameCounter = 0;
	this->FpsObject.Reset();
	this->FpsRaportObject.Reset();
	this->UpdateWindowsTitle();
	this->IntervalRaport = Interval();
}
void GLMgr::UpdateWindowsTitle()
{
	char *prefix;
	if (this->IsGPUMode())
		prefix = "RayTracing GPU";
	else
		prefix = "RayTracing CPU";

	char buff[100];
	sprintf(buff, "%s | Depth: %d | Frames: %d | FPS: %d", prefix, this->RayTracerDepth, this->FrameCounter, this->FpsObject.GetValue());
	glutSetWindowTitle(buff);
}
void GLMgr::UpdateCounter()
{
	this->UpdateWindowsTitle();
	this->FrameCounter++;
}

void GLMgr::PreInit(int argc, char* argv[])
{
	gltSetWorkingDirectory(argv[0]);
	glutInit(&argc, argv);

	if (argc>1)
		this->SceneLoader = new SceneConfigLoader(argv[1]);
	else
		this->SceneLoader = new SceneConfigLoader("SceneConfig.json");

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
	glutInitWindowSize(WINDOWS_SIZE_X, WINDOWS_SIZE_Y);
	glutCreateWindow("RayTracing Init");
	
	glewInit();

	//wylaczenie synchronizacji 
	wglSwapIntervalEXT(0);
	this->ResetCounter();

	//ustawienie callbackow
	glutDisplayFunc(::RenderScene);
	glutKeyboardFunc(::KeyboardFunc);
	glutSpecialFunc(::KeyboardSpecialFunc);

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}
}

GLMgr::~GLMgr(void)
{
	if (this->Shader_TestHit)
		glDeleteProgram(this->Shader_TestHit);
	this->Shader_TestHit = 0;

	if (this->RenderBuffer_Color)
		glDeleteRenderbuffers(1, &this->RenderBuffer_Color);
	this->RenderBuffer_Color = 0;

	OpenGLHelper::DeleteTexture(&this->TextureBuffer_PositionIndex, &this->Texture_PositionIndex);
	OpenGLHelper::DeleteTexture(&this->TextureBuffer_PositionCords, &this->Texture_PositionCords);
	OpenGLHelper::DeleteTexture(&this->TextureBuffer_NormalIndex, &this->Texture_NormalIndex);
	OpenGLHelper::DeleteTexture(&this->TextureBuffer_NormalCords, &this->Texture_NormalCords);
	OpenGLHelper::DeleteTexture(&this->TextureBuffer_MaterialIndex, &this->Texture_MaterialIndex);
	OpenGLHelper::DeleteTexture(&this->TextureBuffer_MaterialProperties, &this->Texture_MaterialProperties);

	if (this->CurrentScene)
		delete(this->CurrentScene);
	this->CurrentScene = NULL;

	if (this->Camera)
		delete(this->Camera);
	this->Camera = NULL;

	if (this->SceneLoader)
		delete(this->SceneLoader);
	this->SceneLoader = NULL;
}
GLMgr::GLMgr() 
	:GPUMode(true), RayTracerDepth(1), SceneLoader(NULL)
{
	this->Shader_TestHit = 0;
	this->RenderBuffer_Color = 0;
	this->TextureBuffer_PositionIndex = 0;
	this->Texture_PositionIndex = 0;
	this->TextureBuffer_PositionCords = 0;
	this->Texture_PositionCords = 0;
	this->TextureBuffer_NormalIndex = 0;
	this->Texture_NormalIndex = 0;
	this->TextureBuffer_NormalCords = 0;
	this->Texture_NormalCords = 0;
	this->TextureBuffer_MaterialIndex = 0;
	this->Texture_MaterialIndex = 0;
	this->TextureBuffer_MaterialProperties = 0;
	this->Texture_MaterialProperties = 0;

	this->CurrentScene = NULL;
	this->Camera = NULL;
}
