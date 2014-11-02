#include "stdafx.h"
#include "ShaderHelper.h"

GLubyte ShaderHelper::_ShaderBuffer[81920];

bool ShaderHelper::LoadShaderFile(const char *filePath, GLuint shaderId)
{
	GLint shaderLen = 0;
	FILE *fp;
	GLchar *stringPtr[1];

	//otwarcie pliku
	fp = fopen(filePath, "r");
	if (fp != NULL)
	{
		//sprawdzenie dlugosci
		while (fgetc(fp) != EOF)
			shaderLen++;

		assert(shaderLen < 81920);  
		if (shaderLen > 81920)
		{
			fclose(fp);
			return false;
		}

		//na poczatek pliku
		rewind(fp);

		//za³adowanie pliku
		if (_ShaderBuffer != NULL)
			fread(_ShaderBuffer, 1, shaderLen, fp);

		_ShaderBuffer[shaderLen] = '\0';
		fclose(fp);
	}
	else
		return false;

	stringPtr[0] = (GLchar *)_ShaderBuffer;
	glShaderSource(shaderId, 1, (const GLchar **)stringPtr, NULL);
	return true;
}
