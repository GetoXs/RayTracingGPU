#pragma once

#include "MetaTypes.h"
#include "Scene.h"
#include "CameraPerspective.h"
#include "CameraOrto.h"
#include "PointLight.h"
#include "qlist.h"
#include "Fps.h"
#include "SceneConfigLoader.h"

//G��wna klasa zarz�dzaj�ca, singleton.
class GLMgr
{
	friend SceneConfigLoader;
private:
	static GLMgr *_instance;
	GLMgr();

	//tryb renderowania sceny.
	bool GPUMode;
	//Warto�c okre�laj�ca poziom g��boko�ci algorytmu.
	unsigned RayTracerDepth;

	//Klasa �aduj�ca plik konfiguracyjny sceny.
	SceneConfigLoader *SceneLoader;

	//licza klatek.
	int FrameCounter;
	//Obiekt s�u�acy do zliczania liczby fpsow.
	Fps FpsObject;
	//Zliczanie okres�w na potrzeby raport�w.
	Interval IntervalRaport;
	//Nazwa wyj�ciowa pliku raport�w.
	QString RaportFilename;

	RectI Viewport;
	//Aktualnie renderowana scena.
	Scene *CurrentScene;
	//Lista �wiate�.
	QList<PointLight*> LightList;
	CameraPerspective *Camera;
	//CameraOrto *Camera;

	//Program cieniujacy.
	GLuint Shader_TestHit;

	//obiekt ramki
	GLuint FrameBufferObject;

	//bufor renderowania koloru
	GLuint RenderBuffer_Color;

	//Obiekt buforu index�w wierzcho�k�w.
	GLuint TextureBuffer_PositionIndex;
	//Obiekt buforu wierzcho�k�w.
	GLuint TextureBuffer_PositionCords;

	//Obiekt tekstury index�w wierzcho�k�w.
	GLuint Texture_PositionIndex;
	//Obiekt tekstury wierzcho�k�w.
	GLuint Texture_PositionCords;

	//Obiekt buforu index�w materia�u wierzcho�k�w.
	GLuint TextureBuffer_MaterialIndex;
	//Obiekt tekstury index�w materia�u wierzcho�k�w.
	GLuint Texture_MaterialIndex;

	//Obiekt buforu w�a�ciwo�ci materia�u.
	GLuint TextureBuffer_MaterialProperties;
	//Obiekt tekstury w�a�ciwo�ci materia�u.
	GLuint Texture_MaterialProperties;

public:
	static GLMgr *I()
	{
		if (_instance == NULL)
			_instance = new GLMgr();
		return _instance;
	}

	CameraPerspective *GetCamera() const { return this->Camera; }

	void PreInit(int argc, char* argv[]);
	void Init();

	//Zmiana aktualnego trybu renderingu.
	void SwitchMode() { this->GPUMode = !this->GPUMode; this->ResetCounter(); }
	bool IsGPUMode() { return this->GPUMode; }
	void SetGPUMode(bool isGPUMode) { this->GPUMode = isGPUMode; this->ResetCounter(); }
	//Metoda rekurencyjna obs�ugi algorytmu �ledzenia promieni na CPU.
	void RayTraceOnCPU(unsigned depth, const Ray *ray, Color *outColor);
	//Metoda obs�ugi algorytmu �ledzenia promieni na CPU.
	void RenderSceneOnCPU(void);
	//Metoda obs�ugi algorytmu �ledzenia promieni na GPU.
	void RenderSceneOnGPU(void);

	//Funkcja raportuj�ca.
	void Raport();
	//Funkcja zapisuj�ca aktualny zrzut ekranu do pliku.
	void SaveDisplayToFile(const char *filename);

	//Zwi�kszanie poziomu g��boko�ci.
	void IncreaseRayTracerDepth() { this->RayTracerDepth++; }
	//Ustawianie poziomu g��boko�ci.
	void SetRayTracerDepth(unsigned depth) { this->RayTracerDepth = depth; }
	//Zmniejszanie poziomu g��boko�ci.
	void DecreaseRayTracerDepth() { if (this->RayTracerDepth >0) this->RayTracerDepth--; }

	//Aktualizacja danych zawratych w pasku okna.
	void UpdateWindowsTitle();
	//Aktualizacja licznik�w.
	void UpdateCounter();
	//Reset licznik�w.
	void ResetCounter();

	~GLMgr(void);
};

