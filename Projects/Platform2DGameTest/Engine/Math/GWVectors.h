#ifndef GWVectors_h__
#define GWVectors_h__

#include <math.h>

class Vector2;
class Vector3;

// 2D Vector
class Vector2
{
public:
	float x, y;

	Vector2() {}
	Vector2(float _x, float _y) : x(_x), y(_y) {}

	inline Vector2 operator+ (const Vector2& rhs) const { return Vector2(x + rhs.x, y + rhs.y); }
	inline Vector2 operator+= (const Vector2& rhs) { x += rhs.x; y += rhs.y; return *this; }
	inline Vector2 operator*= (const Vector2& rhs) { x *= rhs.x; y *= rhs.y; return *this; }

	inline Vector2 operator*= (float val) { x *= val; y *= val; return *this; }

	void Normalize();

	static const Vector2 ZERO;
};

// 3D vector
class Vector3
{
public:
	float x, y, z;

	Vector3() {}
	Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
	Vector3(const Vector2& vec, float _z) : x(vec.x), y(vec.y), z(_z) {}

	inline Vector3 operator+ (const Vector3& rhs) const { return Vector3(x + rhs.x, y + rhs.y, z + rhs.z); }
	inline Vector3 operator+= (const Vector3& rhs) { x += rhs.x; y += rhs.y; z += rhs.z; return *this; }
	inline Vector3 operator*= (const Vector3& rhs) { x *= rhs.x; y *= rhs.y; z *= rhs.z; return *this; }

	inline Vector3 operator*= (float val) { x *= val; y *= val; z *= val; return *this; }

	void Normalize();

	static const Vector3 ZERO;
};


#endif // GWVectors_h__