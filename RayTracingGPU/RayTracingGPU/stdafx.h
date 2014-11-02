#define QT_NO_OPENGL
#include <assert.h>
#include <QtWidgets>
#include <windows.h>		// Must have for Windows platform builds
#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include "gl\glew.h"			// OpenGL Extension "autoloader"
#include "gl\wglew.h"			// OpenGL Extension "autoloader"
#include "gl\GL.h"
#include "ErrorLog.h"

#define FREEGLUT_STATIC
#include <GL/freeglut.h>