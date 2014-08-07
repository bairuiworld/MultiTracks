#ifndef __MULTITRACKS_POINT3D_H__
#define __MULTITRACKS_POINT3D_H__

namespace mt
{
namespace map
{

class Point3D
{
public:
	Point3D(int x_, int y_, int z_) : x(x_), y(y_), z(z_) {}
	Point3D(const Point3D& rhs) : x(rhs.x), y(rhs.y), z(rhs.z) {}

	bool operator ==(const Point3D& rhs) { return x == rhs.x && y == rhs.y && z == rhs.z; }
	bool operator <(const Point3D& rhs) const
	{
		if(x != rhs.x) return x < rhs.x;
		if(y != rhs.y) return y < rhs.y;
		return z < rhs.z;
	}

public:
	int x, y, z;
};

}
}

#endif // !__MULTITRACKS_POINT3D_H__
