#include "stdafx.h"
#include "CameraPerspective.h"


Ray CameraPerspective::GetRay(PointI pixelLocation, RectI viewport)
{
	double x = pixelLocation.x();
	double y = pixelLocation.y();

	//calculate offset
	double xOffset = x - (viewport.width() - 1.0) / 2.0;
	double yOffset = y - (viewport.height() - 1.0) / 2.0;

	//calculate direction
	Vector3D dir = this->ProjectionMatrix*Vector3D(xOffset, yOffset, -Distance);
	dir.normalize();

	return Ray(this->CameraPosition, dir);
}
