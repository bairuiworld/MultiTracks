#define _USE_MATH_DEFINES
#include <cmath>
#include "Algorithm.h"

namespace mt {
namespace math {

double toRadians(double d) { return d*M_PI/180; }
double toDegrees(double r) { return r*180/M_PI; }

}
}