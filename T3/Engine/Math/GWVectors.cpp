/********************************************************************
	created:	2011/9/5
	filename: 	GWVectors.cpp
	author:		Mwolf
	
	purpose:	
*********************************************************************/
#include "GWVectors.h"

const Vector2 Vector2::ZERO = Vector2(0.0f, 0.0f);
const Vector3 Vector3::ZERO = Vector3(0.0f, 0.0f, 0.0f);

void Vector2::Normalize()
{
	float magSq = x*x + y*y;
	if (magSq > 0.0f)
	{
		float oneOverMag = 1.0f / sqrt(magSq);
		x *= oneOverMag;
		y *= oneOverMag;
	}
}

void Vector3::Normalize()
{
	float magSq = x*x + y*y + z*z;
	if (magSq > 0.0f)
	{
		float oneOverMag = 1.0f / sqrt(magSq);
		x *= oneOverMag;
		y *= oneOverMag;
		z *= oneOverMag;
	}
}
