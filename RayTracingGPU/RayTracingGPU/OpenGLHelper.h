#pragma once

#include <stdarg.h>
#include "PointLight.h"


class OpenGLHelper
{
public:
    OpenGLHelper(void);
	static GLuint LoadShader(const char *szVertexProg, const char *szFragmentProg, const char *szGeometryProg, GLsizei feedbackCount, const GLchar** feedbackVars);
    ///tworzy oraz zwraca id programu stworzonego z shadero�w podanych argumentem, dodatkowo ustawia atrybuty oraz bufor sprze�enia zwrotnego je�li nie jest NULLem
    static GLuint LoadShaderWithAttributesAndFeedback(const char *szVertexProg, const char *szFragmentProg, const char *szGeometryProg, GLsizei feedbackCount, const GLchar** feedbackVars, unsigned attCount, ...);
    ///zwraca kod bledu silnika OpenGL
    static GLenum CheckErrors();
	static void DeleteTexture(GLuint *outTextureBuffer, GLuint *outTexture);
	static void RegisterTexture(GLenum textureNumber, void* data, size_t dataSize, GLenum textureFormat, 
		GLuint *outTextureBuffer, GLuint *outTexture);
	static void RegisterLightList(const QList<PointLight*> *lightList);
	static void UpdateLightUniformList(const char* varName, const QList<PointLight*> *lightList, GLuint shaderNum, const Matrix4x4 *transformMatrix);
	static GLuint GetUniformLocation(GLuint shaderNum, char* propertyName);
};

