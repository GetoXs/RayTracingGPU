#include "stdafx.h"
#include "ShaderManager.h"


ShaderManager::ShaderManager()
{
}

bool glLoadShaderFile(const char *szFile, GLuint shader)
{
	GLint shaderLength = 0;
	FILE *fp;

	// Open the shader file
	fp = fopen(szFile, "r");
	if (fp != NULL)
	{
		// See how long the file is
		while (fgetc(fp) != EOF)
			shaderLength++;

		// Allocate a block of memory to send in the shader
		assert(shaderLength < MAX_SHADER_LENGTH);   // make me bigger!
		if (shaderLength > MAX_SHADER_LENGTH)
		{
			fclose(fp);
			return false;
		}

		// Go back to beginning of file
		rewind(fp);

		// Read the whole file in
		if (shaderText != NULL)
			fread(shaderText, 1, shaderLength, fp);

		// Make sure it is null terminated and close the file
		shaderText[shaderLength] = '\0';
		fclose(fp);
	}
	else
		return false;

	// Load the string
	gltLoadShaderSrc((const char *)shaderText, shader);

	return true;
}

ShaderManager::~ShaderManager()
{
}
