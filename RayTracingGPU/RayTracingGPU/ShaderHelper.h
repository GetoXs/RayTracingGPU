#pragma once
///Klasa pomocnicza do obslugi shaderów.
class ShaderHelper
{
private:
public:
	///Metoda obs³uguj¹ca ³adowanie pliku z programem cieniuj¹cym.
	static bool LoadShaderFile(const char *szFile, GLuint shader);
};

