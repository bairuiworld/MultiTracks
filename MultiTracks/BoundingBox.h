#ifndef __MULTITRACKS_BOUNDINGBOX_H__
#define __MULTITRACKS_BOUNDINGBOX_H__

#include <algorithm>
#include <numeric>
#include "Vector.h"

#pragma push_macro("max")
#pragma push_macro("min")
#undef min
#undef max

namespace mt
{

template <class T>
class BoundingBox
{
public:
	BoundingBox(T padding = 0);

	void Add(const Vector<T, 2>& v);
	void Add(T x, T y);

	T GetTop() const { return mTop; }
	T GetLeft() const { return mLeft; }
	T GetBottom() const { return mBottom; }
	T GetRight() const { return mRight; }

	Vector<T, 2> GetTopLeft() const;
	Vector<T, 2> GetBottomRight() const;

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
	mTop = std::min(mTop, y - mPadding);
	mLeft = std::min(mLeft, x - mPadding);
	mBottom = std::max(mBottom, y + mPadding);
	mRight = std::max(mRight, x + mPadding);
}

template <class T>
Vector<T, 2> BoundingBox<T>::GetTopLeft() const
{
	return Vector<T, 2>(mLeft, mTop);
}

template <class T>
Vector<T, 2> BoundingBox<T>::GetBottomRight() const
{
	return Vector<T, 2>(mRight, mBottom);
}

template <class T>
bool BoundingBox<T>::IsInside(const Vector<T, 2>& v)
{
	return IsInside(v.GetX(), v.GetY());
}

template <class T>
bool BoundingBox<T>::IsInside(T x, T y)
{
	return x >= mLeft && x <= mRight && y >= mTop && y <= mBottom;
}

template <class T>
void BoundingBox<T>::SetPadding(T padding)
{
	T delta = padding - mPadding;
	mTop -= delta;
	mLeft -= delta;
	mBottom += delta;
	mRight += delta;
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

#pragma pop_macro("min")
#pragma pop_macro("max")

#endif // !__MULTITRACKS_BOUNDINGBOX_H__