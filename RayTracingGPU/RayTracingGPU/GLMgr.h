#pragma once

#include "MetaTypes.h"
#include "Scene.h"
#include "CameraPerspective.h"
#include "CameraOrto.h"
#include "PointLight.h"
#include "qlist.h"
#include "Fps.h"
#include "SceneConfigLoader.h"

//G³ówna klasa zarz¹dzaj¹ca, singleton.
class GLMgr
{
	friend SceneConfigLoader;
private:
	static GLMgr *_instance;
	GLMgr();

	//tryb renderowania sceny.
	bool GPUMode;
	//Wartoœc okreœlaj¹ca poziom g³êbokoœci algorytmu.
	unsigned RayTracerDepth;

	//Klasa ³aduj¹ca plik konfiguracyjny sceny.
	SceneConfigLoader *SceneLoader;

	//licza klatek.
	int FrameCounter;
	//Obiekt s³u¿acy do zliczania liczby fpsow.
	Fps FpsObject;
	//Zliczanie okresów na potrzeby raportów.
	Interval IntervalRaport;
	//Nazwa wyjœciowa pliku raportów.
	QString RaportFilename;

	RectI Viewport;
	//Aktualnie renderowana scena.
	Scene *CurrentScene;
	//Lista œwiate³.
	QList<PointLight*> LightList;
	CameraPerspective *Camera;
	//CameraOrto *Camera;

	//Program cieniujacy.
	GLuint Shader_TestHit;

	//obiekt ramki
	GLuint FrameBufferObject;

	//bufor renderowania koloru
	GLuint RenderBuffer_Color;

	//Obiekt buforu indexów wierzcho³ków.
	GLuint TextureBuffer_PositionIndex;
	//Obiekt buforu wierzcho³ków.
	GLuint TextureBuffer_PositionCords;

	//Obiekt tekstury indexów wierzcho³ków.
	GLuint Texture_PositionIndex;
	//Obiekt tekstury wierzcho³ków.
	GLuint Texture_PositionCords;

	//Obiekt buforu indexów materia³u wierzcho³ków.
	GLuint TextureBuffer_MaterialIndex;
	//Obiekt tekstury indexów materia³u wierzcho³ków.
	GLuint Texture_MaterialIndex;

	//Obiekt buforu w³aœciwoœci materia³u.
	GLuint TextureBuffer_MaterialProperties;
	//Obiekt tekstury w³aœciwoœci materia³u.
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
	//Metoda rekurencyjna obs³ugi algorytmu œledzenia promieni na CPU.
	void RayTraceOnCPU(unsigned depth, const Ray *ray, Color *outColor);
	//Metoda obs³ugi algorytmu œledzenia promieni na CPU.
	void RenderSceneOnCPU(void);
	//Metoda obs³ugi algorytmu œledzenia promieni na GPU.
	void RenderSceneOnGPU(void);

	//Funkcja raportuj¹ca.
	void Raport();
	//Funkcja zapisuj¹ca aktualny zrzut ekranu do pliku.
	void SaveDisplayToFile(const char *filename);

	//Zwiêkszanie poziomu g³êbokoœci.
	void IncreaseRayTracerDepth() { this->RayTracerDepth++; }
	//Ustawianie poziomu g³êbokoœci.
	void SetRayTracerDepth(unsigned depth) { this->RayTracerDepth = depth; }
	//Zmniejszanie poziomu g³êbokoœci.
	void DecreaseRayTracerDepth() { if (this->RayTracerDepth >0) this->RayTracerDepth--; }

	//Aktualizacja danych zawratych w pasku okna.
	void UpdateWindowsTitle();
	//Aktualizacja liczników.
	void UpdateCounter();
	//Reset liczników.
	void ResetCounter();

	~GLMgr(void);
};

