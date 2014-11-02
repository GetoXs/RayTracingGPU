#include "StdAfx.h"
#include "OpenGLHelper.h"
#include "ShaderHelper.h"


OpenGLHelper::OpenGLHelper(void)
{
}

GLuint OpenGLHelper::LoadShader(
	const char *szVertexProg, const char *szFragmentProg, const char *szGeometryProg,
	GLsizei feedbackCount, const GLchar** feedbackVars)
{
	// Temporary Shader objects
	GLuint hVertexShader;
	GLuint hFragmentShader;
	GLuint hGeometryShader;
	GLuint hReturn = 0;
	GLint testVal;
	bool isGeometryShader = szGeometryProg != NULL;

	// Create shader objects
	hVertexShader = glCreateShader(GL_VERTEX_SHADER);
	hFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	hGeometryShader = glCreateShader(GL_GEOMETRY_SHADER);

	// Load them. If fail clean up and return null
	// Vertex Program
	if (ShaderHelper::LoadShaderFile(szVertexProg, hVertexShader) == false)
	{
		glDeleteShader(hVertexShader);
		glDeleteShader(hFragmentShader);
		glDeleteShader(hGeometryShader);
		fprintf(stderr, "The shader at %s could ot be found.\n", szVertexProg);
		ErrorLog::AppendError("SHADER ERROR", "B��d1 �adowania pliku.");
		return (GLuint)NULL;
	}

	// Fragment Program
	if (ShaderHelper::LoadShaderFile(szFragmentProg, hFragmentShader) == false)
	{
		glDeleteShader(hVertexShader);
		glDeleteShader(hFragmentShader);
		glDeleteShader(hGeometryShader);
		fprintf(stderr, "The shader at %s  could not be found.\n", szFragmentProg);
		ErrorLog::AppendError("SHADER ERROR", "B��d2 �adowania pliku.");
		return (GLuint)NULL;
	}

	//Geometry Shader
	if (isGeometryShader)
	{
		if (ShaderHelper::LoadShaderFile(szGeometryProg, hGeometryShader) == false)
		{
			glDeleteShader(hVertexShader);
			glDeleteShader(hFragmentShader);
			glDeleteShader(hGeometryShader);
			ErrorLog::AppendError("SHADER ERROR", "B��d3 �adowania pliku.");
			return (GLuint)NULL;
		}
	}

	// Compile them both
	glCompileShader(hVertexShader);
	glCompileShader(hFragmentShader);
	if (isGeometryShader)
		glCompileShader(hGeometryShader);

	// Check for errors in vertex shader
	glGetShaderiv(hVertexShader, GL_COMPILE_STATUS, &testVal);
	if (testVal == GL_FALSE)
	{
		char infoLog[1024];
		glGetShaderInfoLog(hVertexShader, 1024, NULL, infoLog);
		fprintf(stderr, "The shader at %s failed to compile with the following error:\n%s\n", szVertexProg, infoLog);
		glDeleteShader(hVertexShader);
		glDeleteShader(hFragmentShader);
		glDeleteShader(hGeometryShader);
		ErrorLog::AppendError("SHADER ERROR", infoLog);
		return (GLuint)NULL;
	}

	// Check for errors in fragment shader
	glGetShaderiv(hFragmentShader, GL_COMPILE_STATUS, &testVal);
	if (testVal == GL_FALSE)
	{
		char infoLog[1024];
		glGetShaderInfoLog(hFragmentShader, 1024, NULL, infoLog);
		fprintf(stderr, "The shader at %s failed to compile with the following error:\n%s\n", szFragmentProg, infoLog);
		glDeleteShader(hVertexShader);
		glDeleteShader(hFragmentShader);
		glDeleteShader(hGeometryShader);
		ErrorLog::AppendError("SHADER ERROR", infoLog);
		return (GLuint)NULL;
	}
	if (isGeometryShader)
	{
		glGetShaderiv(hGeometryShader, GL_COMPILE_STATUS, &testVal);
		if (testVal == GL_FALSE)
		{
			char infoLog[1024];
			glGetShaderInfoLog(hGeometryShader, 1024, NULL, infoLog);
			glDeleteShader(hVertexShader);
			glDeleteShader(hFragmentShader);
			glDeleteShader(hGeometryShader);
			ErrorLog::AppendError("SHADER ERROR", infoLog);
			return (GLuint)NULL;
		}
	}

	// Create the final program object, and attach the shaders
	hReturn = glCreateProgram();
	glAttachShader(hReturn, hVertexShader);
	glAttachShader(hReturn, hFragmentShader);
	if (isGeometryShader)
		glAttachShader(hReturn, hGeometryShader);

	//dodawanie buforow zwrotnych
	if (feedbackCount != 0 && feedbackVars != nullptr)
	{
		glTransformFeedbackVaryings(hReturn, feedbackCount, feedbackVars, GL_SEPARATE_ATTRIBS);
	}

	// Attempt to link    
	glLinkProgram(hReturn);

	// These are no longer needed
	glDeleteShader(hVertexShader);
	glDeleteShader(hFragmentShader);
	glDeleteShader(hGeometryShader);

	// Make sure link worked too
	glGetProgramiv(hReturn, GL_LINK_STATUS, &testVal);
	if (testVal == GL_FALSE)
	{
		char infoLog[1024];
		glGetProgramInfoLog(hReturn, 1024, NULL, infoLog);
		fprintf(stderr, "The programs %s and %s failed to link with the following errors:\n%s\n",
			szVertexProg, szFragmentProg, infoLog);
		glDeleteProgram(hReturn);
		ErrorLog::AppendError("SHADER ERROR", infoLog);
		return (GLuint)NULL;
	}

	// All done, return our ready to use shader program
	return hReturn;
}
GLuint OpenGLHelper::LoadShaderWithAttributesAndFeedback(
	const char *szVertexProg, const char *szFragmentProg, const char *szGeometryProg, 
	GLsizei feedbackCount, const GLchar** feedbackVars, 
	unsigned attCount, ...)
{
	// Temporary Shader objects
	GLuint hVertexShader;
	GLuint hFragmentShader; 
	GLuint hGeometryShader; 
	GLuint hReturn = 0;   
	GLint testVal;
	bool isGeometryShader = szGeometryProg!=NULL;
	
	// Create shader objects
	hVertexShader = glCreateShader(GL_VERTEX_SHADER);
	hFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	hGeometryShader = glCreateShader(GL_GEOMETRY_SHADER);
	
	// Load them. If fail clean up and return null
	// Vertex Program
	if (ShaderHelper::LoadShaderFile(szVertexProg, hVertexShader) == false)
	{
		glDeleteShader(hVertexShader);
		glDeleteShader(hFragmentShader);
		glDeleteShader(hGeometryShader);
		fprintf(stderr, "The shader at %s could ot be found.\n", szVertexProg);
		ErrorLog::AppendError("SHADER ERROR", "The shader at %s could ot be found.");
		return (GLuint)NULL;
	}
	
	// Fragment Program
	if (ShaderHelper::LoadShaderFile(szFragmentProg, hFragmentShader) == false)
	{
		glDeleteShader(hVertexShader);
		glDeleteShader(hFragmentShader);
		glDeleteShader(hGeometryShader);
		fprintf(stderr,"The shader at %s  could not be found.\n", szFragmentProg);
		ErrorLog::AppendError("SHADER ERROR", "The shader at %s  could not be found.");
		return (GLuint)NULL;
	}

	//Geometry Shader
	if(isGeometryShader!=NULL)
	{
		if (ShaderHelper::LoadShaderFile(szGeometryProg, hGeometryShader) == false)
		{
			glDeleteShader(hVertexShader);
			glDeleteShader(hFragmentShader);
			glDeleteShader(hGeometryShader);
			ErrorLog::AppendError("SHADER ERROR", "Blad ladowania pliku.");
			return (GLuint)NULL;
		}
	}
		
	// Compile them both
	glCompileShader(hVertexShader);
	glCompileShader(hFragmentShader);
	if(isGeometryShader)
		glCompileShader(hGeometryShader);
		
	// Check for errors in vertex shader
	glGetShaderiv(hVertexShader, GL_COMPILE_STATUS, &testVal);
	if(testVal == GL_FALSE)
	{
		char infoLog[1024];
		glGetShaderInfoLog(hVertexShader, 1024, NULL, infoLog);
		fprintf(stderr, "The shader at %s failed to compile with the following error:\n%s\n", szVertexProg, infoLog);
		glDeleteShader(hVertexShader);
		glDeleteShader(hFragmentShader);
		glDeleteShader(hGeometryShader);
		ErrorLog::AppendError("SHADER ERROR", infoLog);
		return (GLuint)NULL;
	}
		
	// Check for errors in fragment shader
	glGetShaderiv(hFragmentShader, GL_COMPILE_STATUS, &testVal);
	if(testVal == GL_FALSE)
	{
		char infoLog[1024];
		glGetShaderInfoLog(hFragmentShader, 1024, NULL, infoLog);
		fprintf(stderr, "The shader at %s failed to compile with the following error:\n%s\n", szFragmentProg, infoLog);
		glDeleteShader(hVertexShader);
		glDeleteShader(hFragmentShader);
		glDeleteShader(hGeometryShader);
		ErrorLog::AppendError("SHADER ERROR", infoLog);
		return (GLuint)NULL;
	}
	if(isGeometryShader)
	{
		glGetShaderiv(hGeometryShader, GL_COMPILE_STATUS, &testVal);
		if(testVal == GL_FALSE)
		{
			char infoLog[1024];
			glGetShaderInfoLog(hGeometryShader, 1024, NULL, infoLog);
			glDeleteShader(hVertexShader);
			glDeleteShader(hFragmentShader);
			glDeleteShader(hGeometryShader);
			ErrorLog::AppendError("SHADER ERROR", infoLog);
			return (GLuint)NULL;
		}
	}
		
	// Create the final program object, and attach the shaders
	hReturn = glCreateProgram();
	glAttachShader(hReturn, hVertexShader);
	glAttachShader(hReturn, hFragmentShader);
	if(isGeometryShader)
		glAttachShader(hReturn, hGeometryShader);

	// Now, we need to bind the attribute names to their specific locations
	// List of attributes
	va_list attributeList;
	va_start(attributeList, attCount);

	// Iterate over this argument list
	char *szNextArg;
	int iArgCount = attCount;	// Number of attributes
	for(int i = 0; i < iArgCount; i++)
	{
		int index = va_arg(attributeList, int);
		szNextArg = va_arg(attributeList, char*);
		glBindAttribLocation(hReturn, index, szNextArg);
	}
	va_end(attributeList);
	//glBindAttribLocation(hReturn, atrType, atrName);


	//dodawanie buforow zwrotnych
	if(feedbackCount != 0 && feedbackVars != nullptr)
	{
		glTransformFeedbackVaryings(hReturn, feedbackCount, feedbackVars, GL_SEPARATE_ATTRIBS);
	}

	// Attempt to link    
	glLinkProgram(hReturn);
	
	// These are no longer needed
	glDeleteShader(hVertexShader);
	glDeleteShader(hFragmentShader);
	glDeleteShader(hGeometryShader);
		
	// Make sure link worked too
	glGetProgramiv(hReturn, GL_LINK_STATUS, &testVal);
	if(testVal == GL_FALSE)
	{
		char infoLog[1024];
		glGetProgramInfoLog(hReturn, 1024, NULL, infoLog);
		fprintf(stderr,"The programs %s and %s failed to link with the following errors:\n%s\n",
			szVertexProg, szFragmentProg, infoLog);
		glDeleteProgram(hReturn);
		ErrorLog::AppendError("SHADER ERROR", infoLog);
		return (GLuint)NULL;
	}
		
	// All done, return our ready to use shader program
	return hReturn;  
}  
GLenum OpenGLHelper::CheckErrors()
{
	//GLenum error = 0;
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		ErrorLog::AppendError("OpenGLHelper::CheckErrors");
		assert(error != GL_NO_ERROR);
		int i = 0;
	}
	return error;
}

void OpenGLHelper::RegisterTexture(GLenum textureNumber, void* data, size_t dataSize, GLenum textureFormat, 
	GLuint *outTextureBuffer, GLuint *outTexture)
{
	//generowanie obiektu bufora
	glGenBuffers(1, outTextureBuffer);
	glBindBuffer(GL_TEXTURE_BUFFER, *outTextureBuffer);
	glBufferData(GL_TEXTURE_BUFFER, dataSize, data, GL_STATIC_DRAW);
	OpenGLHelper::CheckErrors();

	//ustawianie aktualnej textury
	glActiveTexture(textureNumber);

	//generowanie textury
	glGenTextures(1, outTexture);
	glBindTexture(GL_TEXTURE_BUFFER, *outTexture);
	glTexBuffer(GL_TEXTURE_BUFFER, textureFormat, *outTexture);
	OpenGLHelper::CheckErrors();
}
//kasowanie tesktury z bufforem i zerowaniem zmiennej
void OpenGLHelper::DeleteTexture(GLuint *outTextureBuffer, GLuint *outTexture)
{
	if (*outTextureBuffer)
		glDeleteBuffers(1, outTextureBuffer);
	outTextureBuffer = 0;
	if (*outTexture)
		glDeleteTextures(1, outTexture);
	outTexture  = 0;
}

/**
	Stary spos�b na rejestracje �r�de� �wiat�a (depricated od 140).
*/
void OpenGLHelper::RegisterLightList(const QList<PointLight*> *lightList)
{
	int lightNum = GL_LIGHT0;
	for each (PointLight *light in *lightList)
	{
		glEnable(lightNum);
		glLightfv(lightNum, GL_POSITION, new float[] {light->Position.x(), light->Position.y(), light->Position.z(), 1.0});
		glLightfv(lightNum, GL_AMBIENT, light->LightAmbientColor.GetValue());
		glLightfv(lightNum, GL_DIFFUSE, light->LightDiffuseColor.GetValue());
		lightNum++;
	}
}
float* GetVector3DData(const Vector3D *pos, bool withW = false)
{
	float *col;
	if (withW)
		col = new float[4];
	else
		col = new float[3];
	col[0] = pos->x();
	col[1] = pos->y();
	col[2] = pos->z();
	if (withW)
		col[3] = 1.0;
	return col;
}
/**
	Wymaga ustawionego programu cieniuj�cego.
*/
void OpenGLHelper::UpdateLightUniformList(const char* varName, const QList<PointLight*> *lightList, GLuint shaderNum, const Matrix4x4 *transformMatrix)
{
	int i = 0;
	for each (PointLight *light in *lightList)
	{
		Vector3D pos = light->Position;
		if (transformMatrix != NULL)
		{
			pos = (*transformMatrix) * pos;
			//pos.normalize();
		}
		char str[50];
		sprintf(str, "%s[%d].position", varName, i);
		glUniform4fv(OpenGLHelper::GetUniformLocation(shaderNum, str), 1, GetVector3DData(&pos, true));
		OpenGLHelper::CheckErrors();
		sprintf(str, "%s[%d].diffuse", varName, i);
		glUniform4fv(OpenGLHelper::GetUniformLocation(shaderNum, str), 1, light->LightDiffuseColor.GetValue());
		OpenGLHelper::CheckErrors();
		sprintf(str, "%s[%d].ambient", varName, i);
		glUniform4fv(OpenGLHelper::GetUniformLocation(shaderNum, str), 1, light->LightAmbientColor.GetValue());
		OpenGLHelper::CheckErrors();
		i++;
	}
	glUniform1ui(OpenGLHelper::GetUniformLocation(shaderNum, "LightNum"), lightList->count());
	OpenGLHelper::CheckErrors();
}

GLuint OpenGLHelper::GetUniformLocation(GLuint shaderNum, char* propertyName)
{
	GLuint result = glGetUniformLocation(shaderNum, propertyName);
	if (result == GL_INVALID_INDEX)
	{
		int i = 0;
	}
	return result;
}