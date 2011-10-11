#include "BoundBox.h"
#include "Math/GWVectors.h"

bool BoundBox::IsPointInsideBox( float x, float y ) const
{
	return xMin < x && xMax > x && yMin < y && yMax > y;
}

bool BoundBox::TestBoxCollision( const BoundBox& box ) const
{
	return this->xMax > box.xMin && this->xMin < box.xMax &&
		   this->yMax > box.yMin && this->yMin < box.yMax;
}

BoundBox BoundBox::Translate( const Vector3& rel ) const
{
	return BoundBox(this->xMin + rel.x,
					this->yMin + rel.y,
					this->xMax + rel.x,
					this->yMax + rel.y);
}
