#pragma once
class ShaderHelper
{
private:
public:
	//Metoda obsługująca ładowanie pliku z programem cieniującym.
	static bool LoadShaderFile(const char *szFile, GLuint shader);
};

