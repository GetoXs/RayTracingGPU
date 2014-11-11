#include "stdafx.h"
#include "ShaderHelper.h"

bool ShaderHelper::LoadShaderFile(const char *filePath, GLuint shaderId)
{
	GLint shaderLen = 0;
	FILE *fp;
	GLchar *stringPtr[1];
	GLubyte *fileBuffer;

	//otwarcie pliku
	fp = fopen(filePath, "r");
	if (fp != NULL)
	{
		//sprawdzenie dlugosci
		while (fgetc(fp) != EOF)
			shaderLen++;

		fileBuffer = new GLubyte[shaderLen+1];
		rewind(fp);

		//za³adowanie pliku
		if (fileBuffer != NULL)
			fread(fileBuffer, 1, shaderLen, fp);

		fileBuffer[shaderLen] = '\0';
		fclose(fp);
	}
	else
		return false;

	stringPtr[0] = (GLchar *)fileBuffer;
	glShaderSource(shaderId, 1, (const GLchar **)stringPtr, NULL);
	delete[] fileBuffer;
	return true;
}
