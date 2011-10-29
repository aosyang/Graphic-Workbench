/********************************************************************
	created:	2011/9/5
	filename: 	BoundBox.cpp
	author:		Mwolf
	
	purpose:	
*********************************************************************/
#include "BoundBox.h"
#include "Math/GWVectors.h"

bool BoundBox::IsPointInsideBox( const Vector2& point ) const
{
	return xMin < point.x && xMax > point.x && yMin < point.y && yMax > point.y;
}

bool BoundBox::TestBoxCollision( const BoundBox& box ) const
{
	return this->xMax > box.xMin && this->xMin < box.xMax &&
		   this->yMax > box.yMin && this->yMin < box.yMax;
}

Vector2 BoundBox::GetCentrePoint() const
{
	return Vector2(0.5f * (xMin + xMax), 0.5f * (yMin + yMax));
}

BoundBox BoundBox::Translate( const Vector2& rel ) const
{
	return BoundBox(this->xMin + rel.x,
					this->yMin + rel.y,
					this->xMax + rel.x,
					this->yMax + rel.y);
}
