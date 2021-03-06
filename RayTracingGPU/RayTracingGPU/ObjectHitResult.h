#pragma once
#include "MetaTypes.h"
///Klasa zawierająca wynik trafienia dla jednego obiektu.
class ObjectHitResult
{
public:
	///Wynik działania.
	bool Result;
	///Minimalny dystans.
	double MinDistance;
	///Normalna w punkcie.
	Vector3D HitNormal;
	///Trafiony punkt.
	Vector3D HitPoint;

	ObjectHitResult() :Result(false), MinDistance(DBL_MAX), HitNormal()
	{
	}
};

