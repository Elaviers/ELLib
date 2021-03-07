#pragma once
#include "Vector2.hpp"

template <typename T>
struct Rect
{
	T x;
	T y;
	T w;
	T h;
	
	Rect(const T& x, const T& y, const T& w, const T& h) : x(x), y(y), w(w), h(h) {}
	Rect(const T& w, const T& h) : x((T)0), y((T)0), w(w), h(h) {}
	Rect() : Rect((T)0, (T)0) {}
};

using RectF = Rect<float>;
