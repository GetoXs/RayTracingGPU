#pragma once

#include <stdarg.h>
#include "PointLight.h"

//Klasa pomocnicza do funkcji OpenGLa.
class OpenGLHelper
{
public:
    OpenGLHelper(void);
	//Metoda ³aduj¹ca shader, wraz z mo¿liwoœci¹ ustawienia buforów zwrotnych.
	static GLuint LoadShader(const char *szVertexProg, const char *szFragmentProg, const char *szGeometryProg, GLsizei feedbackCount, const GLchar** feedbackVars);
    ///tworzy oraz zwraca id programu stworzonego z shaderoów podanych argumentem, dodatkowo ustawia atrybuty oraz bufor sprze¿enia zwrotnego jeœli nie jest NULLem
    static GLuint LoadShaderWithAttributesAndFeedback(const char *szVertexProg, const char *szFragmentProg, const char *szGeometryProg, GLsizei feedbackCount, const GLchar** feedbackVars, unsigned attCount, ...);
    ///zwraca kod bledu silnika OpenGL
    static GLenum CheckErrors();
	//Metoda kasuj¹ca obiekt tekstury.
	static void DeleteTexture(GLuint *outTextureBuffer, GLuint *outTexture);
	//Metoda rejestruj¹ca teskturê w tym inicjalizuj¹ca bufor.
	static void RegisterTexture(GLenum textureNumber, void* data, size_t dataSize, GLenum textureFormat, GLuint *outTextureBuffer, GLuint *outTexture);
	//Rejestracja listy œwiete³ w OpenGLu
	static void RegisterLightList(const QList<PointLight*> *lightList);
	//Aktualizacja obiektów uniform dla œwiate³.
	static void UpdateLightUniformList(const char* varName, const QList<PointLight*> *lightList, GLuint shaderNum, const Matrix4x4 *transformMatrix);
	//Pobranie lokalizacji obiektu uniform.
	static GLuint GetUniformLocation(GLuint shaderNum, char* propertyName);
};

