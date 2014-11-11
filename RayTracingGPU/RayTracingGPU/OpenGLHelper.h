#pragma once

#include <stdarg.h>
#include "PointLight.h"

//Klasa pomocnicza do funkcji OpenGLa.
class OpenGLHelper
{
public:
    OpenGLHelper(void);
	//Metoda �aduj�ca shader, wraz z mo�liwo�ci� ustawienia bufor�w zwrotnych.
	static GLuint LoadShader(const char *szVertexProg, const char *szFragmentProg, const char *szGeometryProg, GLsizei feedbackCount, const GLchar** feedbackVars);
    ///tworzy oraz zwraca id programu stworzonego z shadero�w podanych argumentem, dodatkowo ustawia atrybuty oraz bufor sprze�enia zwrotnego je�li nie jest NULLem
    static GLuint LoadShaderWithAttributesAndFeedback(const char *szVertexProg, const char *szFragmentProg, const char *szGeometryProg, GLsizei feedbackCount, const GLchar** feedbackVars, unsigned attCount, ...);
    ///zwraca kod bledu silnika OpenGL
    static GLenum CheckErrors();
	//Metoda kasuj�ca obiekt tekstury.
	static void DeleteTexture(GLuint *outTextureBuffer, GLuint *outTexture);
	//Metoda rejestruj�ca tesktur� w tym inicjalizuj�ca bufor.
	static void RegisterTexture(GLenum textureNumber, void* data, size_t dataSize, GLenum textureFormat, GLuint *outTextureBuffer, GLuint *outTexture);
	//Rejestracja listy �wiete� w OpenGLu
	static void RegisterLightList(const QList<PointLight*> *lightList);
	//Aktualizacja obiekt�w uniform dla �wiate�.
	static void UpdateLightUniformList(const char* varName, const QList<PointLight*> *lightList, GLuint shaderNum, const Matrix4x4 *transformMatrix);
	//Pobranie lokalizacji obiektu uniform.
	static GLuint GetUniformLocation(GLuint shaderNum, char* propertyName);
};

