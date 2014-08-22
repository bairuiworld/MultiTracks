#define _USE_MATH_DEFINES
#include <cmath>
#include "Algorithm.h"

namespace mt {

double toRadians(double d) { return d*M_PI/180; }
double toDegrees(double r) { return r*180/M_PI; }

double DistanceToSegment(const Vector2d& p, const Vector2d& a, const Vector2d& b, Vector2d* nearest)
{
	if(p == a) { if(nearest != nullptr) nearest->Set(a); return 0; }
	if(p == b) { if(nearest != nullptr) nearest->Set(b); return 0; }
	Vector2d ab(b.GetX() - a.GetX(), b.GetY() - a.GetY());
	Vector2d ap(p.GetX() - a.GetX(), p.GetY() - a.GetY());
	double abap = ab.GetX()*ap.GetX() + ab.GetY()*ap.GetY();
	if(abap < 0) { if(nearest != nullptr) nearest->Set(a); return p.GetDistance(a); }
	double absq = SQ(ab.GetX()) + SQ(ab.GetY());
	double ahsq = SQ(abap)/(SQ(ab.GetX()) + SQ(ab.GetY()));
	if(ahsq > absq) { if(nearest != nullptr) nearest->Set(b); return p.GetDistance(b); }
	if(nearest != nullptr)
	{
		double normab = a.GetDistance(b);
		double ah = std::sqrt(ahsq);
		nearest->Set({a.GetX() + ah*ab.GetX()/normab, a.GetY() + ah*ab.GetY()/normab});
	}
	return std::sqrt(SQ(ap.GetX()) + SQ(ap.GetY()) - ahsq);
}

}