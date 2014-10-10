#include "stdafx.h"
#include "raytracinggpu.h"

RayTracingGPU::RayTracingGPU(int argc, char *argv[], QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	
	GLMgr::I()->PreInit(argc, argv);

	GLMgr::I()->Init();
}

RayTracingGPU::~RayTracingGPU()
{

}
