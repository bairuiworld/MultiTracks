#ifndef __MULTITRACKS_BOUNDINGBOX_H__
#define __MULTITRACKS_BOUNDINGBOX_H__

#include <algorithm>
#include <numeric>
#include "Vector.h"

namespace mt
{

template <class T>
class BoundingBox
{
public:
	BoundingBox(T padding = 0);

	void Add(const Vector<T, 2>& v);
	void Add(T x, T y);

	bool IsInside(const Vector<T, 2>& v);
	bool IsInside(T x, T y);

	void SetPadding(T padding);
	void Clear();

private:
	T mPadding;
	T mTop, mLeft, mRight, mBottom;
};

template <class T>
BoundingBox<T>::BoundingBox(T padding) : mPadding(padding)
{
	Clear();
}

template <class T>
void BoundingBox<T>::Add(const Vector<T, 2>& v)
{
	Add(v.GetX(), v.GetY());
}

template <class T>
void BoundingBox<T>::Add(T x, T y)
{
	mTop = std::min(mTop, y);
	mLeft = std::min(mLeft, x);
	mBottom = std::max(mBottom, y);
	mRight = std::max(mRight, x);
}

template <class T>
bool BoundingBox<T>::IsInside(const Vector<T, 2>& v)
{
	return IsInside(v.GetX(), v.GetY());
}

template <class T>
bool BoundingBox<T>::IsInside(T x, T y)
{
	return x >= mLeft - mPadding && x <= mRight + mPadding && y >= mTop - mPadding && y <= mBottom + mPadding;
}

template <class T>
void BoundingBox<T>::SetPadding(T padding)
{
	mPadding = padding;
}

template <class T>
void BoundingBox<T>::Clear()
{
	mTop = std::numeric_limits<T>::max();
	mLeft = std::numeric_limits<T>::max();
	mBottom = std::numeric_limits<T>::min();
	mRight = std::numeric_limits<T>::min();
}

}

#endif // !__MULTITRACKS_BOUNDINGBOX_H__