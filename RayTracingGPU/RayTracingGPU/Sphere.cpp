#include "stdafx.h"
#include "Sphere.h"

ObjectHitResult Sphere::HitTest(Ray *ray)
{
	ObjectHitResult result;
	Vector3D vDistance = (ray->Start - this->Center);
	//a = d•d
	double A = ray->Direction.lengthSquared();
	//b = 2(o-c)•d
	double B = 2 * Vector3D::dotProduct(vDistance, ray->Direction);
	//c = (o - c)•(o - c) - r ^ 2
	double C = vDistance.lengthSquared() - (this->Radius * this->Radius);

	double delta = B*B - 4 * A*C;
	if (delta >= 0)
	{	//s¹ przeciêcia
		double sqrtDelta = sqrt(delta);
		double t1 = (-B + sqrtDelta) / (2 * A);
		double t2 = (-B - sqrtDelta) / (2 * A);
		if (t1 >= DBL_EPSILON || t2 >= DBL_EPSILON)
		{
			if (t1 < t2)
				result.MinDistance = t1;
			else
				result.MinDistance = t2; 
			result.Result = true;
		}
	}

	//calculate hit point and normal
	if(result.Result == true)
	{
		result.HitPoint = ray->Start + ray->Direction * result.MinDistance;
		result.HitNormal = (result.HitPoint - this->Center).normalized();
	}
	return result;
}
