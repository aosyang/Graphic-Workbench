#ifndef BoundBox_h__
#define BoundBox_h__

class Vector2;
class Vector3;

class BoundBox
{
public:
	float xMin, yMin;
	float xMax, yMax;

	BoundBox()
		: xMin(0.0f), yMin(0.0f), xMax(0.0f), yMax(0.0f)
	{}

	BoundBox(float _xMin, float _yMin, float _xMax, float _yMax)
		: xMin(_xMin), yMin(_yMin), xMax(_xMax), yMax(_yMax)
	{}

	bool IsPointInsideBox(const Vector2& point) const;
	bool TestBoxCollision(const BoundBox& box) const;
	Vector2 GetCentrePoint() const;

	BoundBox Translate(const Vector3& rel) const;
};

#endif // BoundBox_h__
