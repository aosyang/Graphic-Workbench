#ifndef Vectors_h__
#define Vectors_h__

#include "MathUtil.h"

class Vector3
{
public:
	float x, y, z;

	Vector3(void){};
	Vector3(const Vector3 &a):x(a.x), y(a.y), z(a.z){}
	Vector3(float nx, float ny, float nz):x(nx), y(ny), z(nz){}
	Vector3(const float vec[3])
	{
		x = vec[0];
		y = vec[1];
		z = vec[2];
	}

	Vector3 &operator = (const Vector3 &a)
	{
		x = a.x; y = a.y; z = a.z;
		return *this;
	}
	bool operator ==(const Vector3 &a) const{return FLOAT_EQUAL(x, a.x) && FLOAT_EQUAL(y, a.y) && FLOAT_EQUAL(z, a.z);}
	bool operator !=(const Vector3 &a) const{return !FLOAT_EQUAL(x, a.x) || !FLOAT_EQUAL(y, a.y) || !FLOAT_EQUAL(z, a.z);}

	void zero(){ x = y = z = 0.0f;}
	Vector3 operator - () const {return Vector3 (-x, -y, -z);}
	Vector3 operator + (const Vector3 &a) const { return Vector3 (x + a.x, y + a.y, z + a.z);}
	Vector3 operator - (const Vector3 &a) const { return Vector3 (x - a.x, y - a.y, z - a.z);}
	Vector3 operator * (float a) const { return Vector3 (x * a, y * a, z * a);}
	Vector3 operator / (float a) const { float oneOverA = 1.0f / a; return Vector3(x * oneOverA, y * oneOverA, z * oneOverA);}

	Vector3 &operator += (const Vector3 &a) { x += a.x; y += a.y; z += a.z;return *this;}
	Vector3 &operator -= (const Vector3 &a) { x -= a.x; y -= a.y; z -= a.z;return *this;}
	Vector3 &operator *= (float a) { x *= a; y *= a; z *= a;return *this;}
	Vector3 &operator /= (float a) { float oneOverA = 1.0f / a; x *= oneOverA; y *= oneOverA; z *= oneOverA; return *this;}

	inline float operator [] ( const size_t i ) const { return *(&x+i);	}
	inline float& operator [] ( const size_t i ) { return *(&x+i);	}

	inline float SquaredLength() const { return x * x + y * y + z * z; }
	inline float Length() const { return sqrt(SquaredLength()); }

	inline const float* GetArray() const { return &x; }

	void Normalize()
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

	float operator *(const Vector3 &a) const { return x * a.x + y * a.y + z * a.z; }
};

#endif // Vectors_h__