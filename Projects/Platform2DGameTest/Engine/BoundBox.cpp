#include "BoundBox.h"

bool BoundBox::IsPointInsideBox( float x, float y )
{
	return xMin < x && xMax > x && yMin < y && yMax > y;
}
