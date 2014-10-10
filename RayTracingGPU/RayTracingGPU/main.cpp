#include "stdafx.h"
#include "raytracinggpu.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication::addLibraryPath("plugins");
	QApplication a(argc, argv);
	RayTracingGPU w(argc, argv);
	w.show();
	return a.exec();
}
