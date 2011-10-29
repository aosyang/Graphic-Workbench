/********************************************************************
	created:	2011/9/5
	filename: 	BoundBox.h
	author:		Mwolf
	
	purpose:	
*********************************************************************/
#ifndef BoundBox_h__
#define BoundBox_h__

#include "Math/GWVectors.h"

class BoundBox
{
public:
	float xMin, yMin;
	float xMax, yMax;

	BoundBox()
		: xMin(0.0f), yMin(0.0f), xMax(0.0f), yMax(0.0f)
	{}

	BoundBox(const Vector2& vMin, const Vector2& vMax)
		: xMin(vMin.x), yMin(vMin.y), xMax(vMax.x), yMax(vMax.y)
	{}

	BoundBox(float _xMin, float _yMin, float _xMax, float _yMax)
		: xMin(_xMin), yMin(_yMin), xMax(_xMax), yMax(_yMax)
	{}

	bool IsPointInsideBox(const Vector2& point) const;
	bool TestBoxCollision(const BoundBox& box) const;
	Vector2 GetCentrePoint() const;

	Vector2 vMin() const { return Vector2(xMin, yMin); }
	Vector2 vMax() const { return Vector2(xMax, yMax); }

	BoundBox Translate(const Vector2& rel) const;
};

#endif // BoundBox_h__
