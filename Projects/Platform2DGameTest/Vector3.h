#ifndef Vector3_h__
#define Vector3_h__

#include <math.h>

// 3D vector
class Vector3
{
public:
	float x, y, z;

	Vector3() {}
	Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

	Vector3 operator+ (const Vector3& rhs) const { return Vector3(x + rhs.x, y + rhs.y, z + rhs.z); }
	Vector3 operator+= (const Vector3& rhs) { x += rhs.x; y += rhs.y; z += rhs.z; return *this; }
	Vector3 operator*= (const Vector3& rhs) { x *= rhs.x; y *= rhs.y; z *= rhs.z; return *this; }

	Vector3 operator*= (float val) { x *= val; y *= val; z *= val; return *this; }

	void Normalize();

	static const Vector3 ZERO;
};


#endif // Vector3_h__