#include "Vector3.h"

const Vector3 Vector3::ZERO = Vector3(0.0f, 0.0f, 0.0f);

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
