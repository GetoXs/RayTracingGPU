#include "stdafx.h"
#include "CameraOrto.h"

Ray CameraOrto::GetRay(PointI screenLocation)
{
	// Kierunek w kt�rym skierowane s� wszystkie promienie
	// wychodz�ce z kamery.
	// Otrzymany prostymi funkcjami trygonometrycznymi.
	Vector3D direction(
		sin(this->Angle),
		0,
		cos(this->Angle));
	// Kierunek promienia zawsze musi by� znormalizowany.
	direction = direction.normalized();

	// po�o�enia kamery
	PointI offsetFromCenter(
		screenLocation.x(),
		screenLocation.y());

	// Obliczenie finalnego po�o�enia kamery,
	// r�wnie� proste funkcje trygonometryczne.
	Vector3D position(
		EyePosition.x() + offsetFromCenter.x() * cos(this->Angle),
		EyePosition.y() + offsetFromCenter.y(),
		EyePosition.z() + offsetFromCenter.x() * sin(this->Angle));

	return Ray(position, direction);
}

