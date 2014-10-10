#pragma once

#include "MetaTypes.h"
#include "Scene.h"
#include "CameraPerspective.h"
#include "CameraOrto.h"
#include "PointLight.h"
#include "qlist.h"
#include "Fps.h";

class GLMgr
{
private:
	static GLMgr *_instance;
	GLMgr();

	int FrameCounter;
	std::string WindowTitle;
	Fps FpsObject;


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

	void UpdateCounter();
	void PreInit(int argc, char* argv[]);
	void Init();
	void RenderScene(void);
	void RenderSceneOnGPU(void);

	~GLMgr(void);
};

