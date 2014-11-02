#ifndef RAYTRACINGGPU_H
#define RAYTRACINGGPU_H

#include <QtWidgets/QMainWindow>
#include "ui_raytracinggpu.h"

class RayTracingGPU : public QMainWindow
{
	Q_OBJECT

public:
	RayTracingGPU(int argc, char *argv[], QWidget *parent = 0);
	~RayTracingGPU();

private:
	
	Ui::RayTracingGPUClass ui;
};

#endif // RAYTRACINGGPU_H
