#pragma once
///Klasa pomocnicza do obslugi shader�w.
class ShaderHelper
{
private:
public:
	///Metoda obs�uguj�ca �adowanie pliku z programem cieniuj�cym.
	static bool LoadShaderFile(const char *szFile, GLuint shader);
};

