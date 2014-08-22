#ifndef __MULTITRACKS_ALGORITHM_H__
#define __MULTITRACKS_ALGORITHM_H__

#include "Vector.h"

namespace mt {

double toRadians(double d);
double toDegrees(double r);

double DistanceToSegment(const Vector2d& p, const Vector2d& a, const Vector2d& b, Vector2d* nearest = nullptr);

}

#endif // __MULTITRACKS_ALGORITHM_H__