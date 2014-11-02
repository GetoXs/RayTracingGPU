#pragma once
class ShaderHelper
{
private:
public:
	static GLubyte _ShaderBuffer[81920];
	static bool LoadShaderFile(const char *szFile, GLuint shader);
};

