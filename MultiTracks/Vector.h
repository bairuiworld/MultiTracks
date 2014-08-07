#ifndef __MULTITRACKS_VECTOR_H__
#define __MULTITRACKS_VECTOR_H__

#include <initializer_list>
#include <cassert>

namespace mt
{
	template <class T, int N>
	class Vector
	{
	public:
		Vector() = default;
		Vector(std::initializer_list<T> l);

		T GetX() const;
		T GetY() const;
		T GetZ() const;

		T operator [](unsigned int i);
		const T operator [](unsigned int i) const;

		void SetX(T v);
		void SetY(T v);
		void SetZ(T v);
		void Set(std::initializer_list<T> l);

		bool operator ==(const Vector<T, N>& rhs) const;
		bool operator !=(const Vector<T, N>& rhs) const;

		bool operator <(const Vector<T, N>& rhs) const;

		void operator =(const Vector<T, N>& rhs);

	private:
		T mValues[N];
	};

	template <class T, int N>
	Vector<T, N>::Vector(std::initializer_list<T> l)
	{
		Set(l);
	}

	template <class T, int N>
	T Vector<T, N>::GetX() const
	{
		static_assert(N >= 1, "Wrong index");
		return mValues[0];
	}

	template <class T, int N>
	T Vector<T, N>::GetY() const
	{
		static_assert(N >= 2, "Wrong index");
		return mValues[1];
	}

	template <class T, int N>
	T Vector<T, N>::GetZ() const
	{
		static_assert(N >= 3, "Wrong index");
		return mValues[2];
	}

	template <class T, int N>
	T Vector<T, N>::operator [](unsigned int i)
	{
		assert(i < N);
		return mValues[i];
	}

	template <class T, int N>
	const T Vector<T, N>::operator [](unsigned int i) const
	{
		assert(i < N);
		return mValues[i];
	}

	template <class T, int N>
	void Vector<T, N>::SetX(T v)
	{
		static_assert(N >= 1, "Wrong index");
		mValues[0] = v;
	}

	template <class T, int N>
	void Vector<T, N>::SetY(T v)
	{
		static_assert(N >= 2, "Wrong index");
		mValues[1] = v;
	}

	template <class T, int N>
	void Vector<T, N>::SetZ(T v)
	{
		static_assert(N >= 3, "Wrong index");
		mValues[2] = v;
	}

	template <class T, int N>
	void Vector<T, N>::Set(std::initializer_list<T> l)
	{
		auto it = l.begin();
		for(int i = 0; i<N, it != l.end(); i++, it++)
			mValues[i] = *it;
	}

	template <class T, int N>
	bool Vector<T, N>::operator ==(const Vector<T, N>& rhs) const
	{
		for(int i = 0; i<N; i++)
			if(mValues[i] != rhs.mValues[i])
				return false;
		return true;
	}

	template <class T, int N>
	bool Vector<T, N>::operator !=(const Vector<T, N>& rhs) const
	{
		for(int i = 0; i<N; i++)
			if(mValues[i] != rhs.mValues[i])
				return true;
		return false;
	}

	template <class T, int N>
	bool Vector<T, N>::operator <(const Vector<T, N>& rhs) const
	{
		for(int i = 0; i<N; i++)
			if(mValues[i] != rhs.mValues[i])
				return mValues[i] < rhs.mValues[i];
		return false;
	}

	template <class T, int N>
	void Vector<T, N>::operator =(const Vector<T, N>& rhs)
	{
		for(int i = 0; i<N; i++)
			mValues[i] = rhs.mValues[i];
	}

	typedef Vector<double, 2> Vector2d;
	typedef Vector<double, 3> Vector3d;
	typedef Vector<float, 2> Vector2f;
	typedef Vector<float, 3> Vector3f;
	typedef Vector<int, 2> Vector2i;
	typedef Vector<int, 3> Vector3i;
}

#endif // !__MULTITRACKS_VECTOR_H__