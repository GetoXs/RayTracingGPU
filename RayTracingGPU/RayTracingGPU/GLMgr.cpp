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


//const char* MESH_FILE1 = "C:\\Users\\Mateusz\\Desktop\\mgr\\proj\\Qt\\RayTracingGPU\\bin\\Debug\\cube.ply";
const char* MESH_FILE1 = "C:\\Users\\Mateusz\\Desktop\\New folder\\assimp--3.0.1270-sdk\\test\\models\\OBJ\\box.obj";
//const char* MESH_FILE1 = "C:\\Users\\Mateusz\\Desktop\\mgr\\proj\\Qt\\RayTracingGPU\\bin\\Debug\\Wuson.ply";
//const char* MESH_FILE1 = "C:\\Users\\Mateusz\\Desktop\\mgr\\proj\\Qt\\RayTracingGPU\\bin\\Debug\\bunny.ply";
//const char* MESH_FILE1 = "Z:\\Programowanie\\OpenGL\\Kurs OpenGl 3.2\\14_opengl_32_zaawansowane_modele_cieniowania_i_oswietlenia\\media\\obj\\teapot.obj";
const char* MESH_FILE2 = "C:\\Users\\Mateusz\\Desktop\\New folder\\assimp--3.0.1270-sdk\\test\\models\\OBJ\\box2.obj";

GLMgr *GLMgr::_instance = NULL;

//static GLFrame				viewFrame;
static GLuint               vao;                // The VAO
static GLuint               vertexBuffer;       // Geometry VBO
static GLTriangleBatch     sphereBatch;

// Geometry for a simple quad
static const GLfloat quad[] =
{
	-1.0f, -1.0f, 0.0f,
	1.0f, -1.0f, 0.0f,
	1.0f, 1.0f, 0.0f,
	-1.0f, 1.0f, 0.0f,
};

#define WINDOWS_SIZE_X (800)
#define WINDOWS_SIZE_Y (800)
#define FOCAL_DIST 1000

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
	float nearPlane = 2, farPlane = 7;
	//kamera
	if (this->Viewport.width() < this->Viewport.height() && this->Viewport.width() > 0)
	{
		this->Camera = new CameraPerspective(
			left*this->Viewport.height() / this->Viewport.width(), 
			right*this->Viewport.height() / this->Viewport.width(), 
			top, 
			bottom,
			nearPlane, farPlane);
	}
	else
	{
		this->Camera = new CameraPerspective(
			left, 
			right, 
			top*this->Viewport.width() / this->Viewport.height(), 
			bottom*this->Viewport.width() / this->Viewport.height(),
			nearPlane, farPlane);
	}
	
	//this->Camera = new CameraOrto(Vector3D(0, 0, -5), PointI(this->Viewport.width(), this->Viewport.height()),
	//	0.0, this->Viewport.width(), 0.0, this->Viewport.height(), -1.0, 1.0);
#pragma endregion

#pragma region Scena
	this->CurrentScene = new Scene();
	unsigned mat1 = this->CurrentScene->AddMaterial(new StandardMaterial(Color(1, 1, 0, 0.4), Color(0.3, 0.3, 0, 0.4), 0));
	unsigned mat2 = this->CurrentScene->AddMaterial(new StandardMaterial(Color(0, 0, 1), Color(0., 0., 0.5), 0.3));
	this->CurrentScene->AddObject(new Mesh(MESH_FILE2, &Vector3D(0, 0, 0)), mat2);
	this->CurrentScene->AddObject(new Mesh(MESH_FILE1, &Vector3D(0, 0, 0)), mat1);

	//this->CurrentScene->AddObject(new Sphere(Vector3D(0, 0, 0), 20, new StandardMaterial(Color(1,1,0), Color(0.2,0.2,0))));
	//this->CurrentScene->AddObject(new Sphere(Vector3D(-40, 0, 0), 20, new StandardMaterial(Color(0,1,0), Color(0,0.2,0))));
	//this->CurrentScene->AddObject(new Sphere(Vector3D(40, 40, 0), 20, new StandardMaterial(Color(0,1,1), Color(0,0.2,0.2))));
	//this->CurrentScene->AddObject(new Sphere(Vector3D(80, -40, 0), 20, new StandardMaterial(Color(1,1,1), Color(0,0.2,0.2))));
	//this->CurrentScene->AddObject(new Sphere(Vector3D(120, 0, 0), 20, new StandardMaterial(Color(0,1,1), Color(0,0.2,0.2))));
#pragma endregion

#pragma region Light
	this->LightList.append(new PointLight(Vector3D(0., -0.2, .6), Color(1.0, 1.0, 1.0), Color(.4, .4, .4)));
	this->LightList.append(new PointLight(Vector3D(0., 0.4, 0), Color(1.0, 0, 0), Color(.4, .0, .0)));
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

	tmpBufferUI = this->CurrentScene->GetPositionIndexData(&buffCount);
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
	OpenGLHelper::RegisterTexture(GL_TEXTURE5, tmpBufferF, buffCount * sizeof(float),
		GL_RGBA32F, &TextureBuffer_MaterialProperties, &Texture_MaterialProperties);
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

#pragma region Ustawienia macierzy OpenGL
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//double aspectRatio = (double)WINDOWS_SIZE_X/WINDOWS_SIZE_Y;
	//double theta = 2.0*atan( 0.5*(double)WINDOWS_SIZE_Y/Camera->Distance );
	//double fH = tan( theta / 360 * 3.14 ) * 1;
	//gluPerspective( 100, aspectRatio, 1.0, 200 );
	//numError = OpenGLHelper::CheckErrors();

	//gluOrtho2D(0.0, this->Viewport.width(), 0.0, this->Viewport.height());

	//Matrix4x4 m;
	//m.ortho(0.0, this->Viewport.width(), 0.0, this->Viewport.height(), 0.0, 1.0);

	//macierz projekcji ustawiona na 2d -> renderowanie perspektywiczne odbywa siê w shaderze
	Matrix4x4 ortho2D = Matrix4x4();
	ortho2D.ortho(this->Viewport.left(), this->Viewport.right(), this->Viewport.top(), this->Viewport.bottom(), -1., 1.);
	glLoadMatrixf(ortho2D.data());

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
#pragma endregion



	Matrix4x4 modelViewMatrix;
	modelViewMatrix.setToIdentity();
	modelViewMatrix.translate(0, 0, -3);
	this->Camera->SetModelViewMatrix(&modelViewMatrix);


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
	}
	if (depth < 1)
	{
		if (hitResult.HitMaterial->IsTransmissive())
		{
			//wyznaczenie parametrów kolejnego promienia
			Vector3D transmissiveDir = -ray->Direction;
			Vector3D transmissiveOrigin = hitResult.HitPoint + transmissiveDir*0.001;
			Color transmissiveColor = Color(0);
			//kolejny test promienia
			this->RayTraceOnCPU(depth + 1, &Ray(transmissiveOrigin, transmissiveDir), &transmissiveColor);
			//color blending
			*outColor = hitResult.HitMaterial->CalculateTransmissionColor(outColor, &transmissiveColor);
			//todo: blending
		}
		//todo: reflective
	}
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
			pixelColor = Color(0, 0, 0, 1);
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
	OpenGLHelper::CheckErrors();
}
void GLMgr::RenderSceneOnGPU()
{
	GLenum numError = OpenGLHelper::CheckErrors();

#pragma region Wypelnienie ramki
	//ustawienie tamki dla tekstury testowej
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->FrameBufferObject);
	//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	OpenGLHelper::CheckErrors();
	//dowiazanie wyjsciowej testury testowej
	GLenum drawBuffer[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, this->RenderBuffer_Color);
	OpenGLHelper::CheckErrors();
	glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, TestOutputTexture, 0);
	OpenGLHelper::CheckErrors();
	glDrawBuffers(sizeof(drawBuffer) / sizeof(GLenum), drawBuffer);
	OpenGLHelper::CheckErrors();
#pragma endregion

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(this->Shader_TestHit);
	numError = OpenGLHelper::CheckErrors();

#pragma region Setting Uniform
	//ustawienie viewportu do kalkulacji wspó³rzêdnych na zmienne lokalne
	glUniform4f(glGetUniformLocation(Shader_TestHit, "Viewport"),
		this->Viewport.left(), this->Viewport.top(), this->Viewport.right(), this->Viewport.bottom());
	numError = OpenGLHelper::CheckErrors();

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


	glUniformMatrix4fv(OpenGLHelper::GetUniformLocation(Shader_TestHit, "ProjectionMatrixInverse"), 1, GL_FALSE,
		modelViewProjInverted.data());
	numError = OpenGLHelper::CheckErrors();

	glUniform1i(OpenGLHelper::GetUniformLocation(Shader_TestHit, "PositionIndexTexture"), 0);
	glUniform1i(OpenGLHelper::GetUniformLocation(Shader_TestHit, "PositionCordsTexture"), 1);
	glUniform1i(OpenGLHelper::GetUniformLocation(Shader_TestHit, "NormalTexture"), 2);
	glUniform1i(OpenGLHelper::GetUniformLocation(Shader_TestHit, "NormalIndexTexture"), 3);
	glUniform1i(OpenGLHelper::GetUniformLocation(Shader_TestHit, "MaterialIndexTexture"), 4);
	glUniform1i(OpenGLHelper::GetUniformLocation(Shader_TestHit, "MaterialPropertiesTexture"), 5);
	OpenGLHelper::UpdateLightUniformList("Light", &this->LightList, Shader_TestHit, NULL);
	numError = OpenGLHelper::CheckErrors();
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
	if (false)
	{
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
	}
	numError = OpenGLHelper::CheckErrors();
#pragma endregion

#pragma region Przekopiowanie ramki na ekran
	glBindFramebuffer(GL_READ_FRAMEBUFFER, this->FrameBufferObject);
	OpenGLHelper::CheckErrors();
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	OpenGLHelper::CheckErrors();
	//powrot do standardowej ramki
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	OpenGLHelper::CheckErrors();
	glBlitFramebuffer(0, 0, this->Viewport.width(), this->Viewport.height(),
		0, 0, this->Viewport.width(), this->Viewport.height(),
		GL_COLOR_BUFFER_BIT, GL_LINEAR);
	OpenGLHelper::CheckErrors();
#pragma endregion

	glutSwapBuffers();
	numError = OpenGLHelper::CheckErrors();
	glutPostRedisplay();
	FpsObject.Update();
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

// Called to draw scene
void KeyboardFunc(unsigned char key, int x, int y)
{
	switch (key) {
	case ' ':
		GLMgr::I()->SwitchMode();
		break;
	}
}


void GLMgr::ResetCounter()
{
	this->FrameCounter = 0;
	this->FpsObject.Reset();
	this->UpdateWindowsTitle();
}
void GLMgr::UpdateWindowsTitle()
{
	char *prefix;
	if (this->IsGPUMode())
		prefix = "RayTracing GPU";
	else
		prefix = "RayTracing CPU";

	char buff[100];
	sprintf(buff, "%s | Frames: %d | FPS: %d", prefix, this->FrameCounter, this->FpsObject.GetValue());
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

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
	glutInitWindowSize(WINDOWS_SIZE_X, WINDOWS_SIZE_Y);
	glutCreateWindow("RayTracing Init");
	this->ResetCounter();
	glutDisplayFunc(::RenderScene);
	glutKeyboardFunc(::KeyboardFunc);

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		// Problem: glewInit failed, something is seriously wrong.
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}
}

GLMgr::~GLMgr(void)
{
	if (this->CurrentScene)
		delete(this->CurrentScene);
	this->CurrentScene = NULL;

	if (this->Camera)
		delete(this->Camera);
	this->Camera = NULL;
}
GLMgr::GLMgr() :GPUMode(false)
{
	this->CurrentScene = NULL;
	this->Camera = NULL;
}
