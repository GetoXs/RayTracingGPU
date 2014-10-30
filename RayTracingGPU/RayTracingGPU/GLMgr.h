#pragma once

#include "MetaTypes.h"
#include "Scene.h"
#include "CameraPerspective.h"
#include "CameraOrto.h"
#include "PointLight.h"
#include "qlist.h"
#include "Fps.h"
#include "SceneConfigLoader.h"

class GLMgr
{
	friend SceneConfigLoader;
private:
	static GLMgr *_instance;
	GLMgr();

	bool GPUMode;

	SceneConfigLoader *SceneLoader;

	int FrameCounter;
	Fps FpsObject;

	unsigned RayTracerDepth;

	RectI Viewport;
	Scene *CurrentScene;
	QList<PointLight*> LightList;
	CameraPerspective *Camera;
	//CameraOrto *Camera;


	GLuint Shader_TestHit;

	//obiekt ramki
	GLuint FrameBufferObject;

	//bufor renderowania koloru
	GLuint RenderBuffer_Color;

	GLuint TextureBuffer_PositionIndex;
	GLuint TextureBuffer_PositionCords;

	GLuint Texture_PositionIndex;
	GLuint Texture_PositionCords;

	GLuint TextureBuffer_NormalIndex;
	GLuint TextureBuffer_NormalCords;

	GLuint Texture_NormalIndex;
	GLuint Texture_NormalCords;

	GLuint TextureBuffer_MaterialIndex;
	GLuint Texture_MaterialIndex;

	GLuint TextureBuffer_MaterialProperties;
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
	void SwitchMode() { this->GPUMode = !this->GPUMode; this->ResetCounter(); }
	bool IsGPUMode() { return this->GPUMode; }
	void RayTraceOnCPU(unsigned depth, const Ray *ray, Color *outColor);
	void RenderSceneOnCPU(void);
	void RenderSceneOnGPU(void);

	void IncreaseRayTracerDepth() { this->RayTracerDepth++; }
	void SetRayTracerDepth(unsigned depth) { this->RayTracerDepth = depth; }
	void DecreaseRayTracerDepth() { if (this->RayTracerDepth >0) this->RayTracerDepth--; }

	void UpdateWindowsTitle();
	void UpdateCounter();
	void ResetCounter();

	~GLMgr(void);
};

