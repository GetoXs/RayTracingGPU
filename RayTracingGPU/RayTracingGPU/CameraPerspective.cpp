#include "stdafx.h"
#include "CameraPerspective.h"

Ray CameraPerspective::GetRay(PointI pixelLocation, RectI viewport)
{
	const Matrix4x4 *modelViewProjectionMatrixInverted = this->GetModelViewProjectionMatrixInverted();

	//przekszta³cenie ze wspó³rzêdnych vp na ortho 2d (-1; 1)
	double xOffset = pixelLocation.x() / ((viewport.width() - 1.0) / 2.0) - 1;
	double yOffset = pixelLocation.y() / ((viewport.height() - 1.0) / 2.0) - 1;

	//ustawianie konca i pocz¹tku obliczanego wektora
	Vector3D farPlaneSrc = Vector3D(xOffset, yOffset, 1);
	Vector3D nearPlaneSrc = Vector3D(xOffset, yOffset, -1);

	//przekszta³cenie ze wspó³rzêdnych ortho na perspektywiczne
	Vector3D farPlaneDst = *modelViewProjectionMatrixInverted*farPlaneSrc;
	Vector3D nearPlaneDst = *modelViewProjectionMatrixInverted*nearPlaneSrc;

	//obliczanie kierunku
	Vector3D dir = (farPlaneDst - nearPlaneDst).normalized();

	return Ray(nearPlaneDst, dir);
}