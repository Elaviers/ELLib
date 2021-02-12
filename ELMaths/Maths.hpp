#pragma once
#include <ELCore/Types.hpp>
#include <ELSys/Debug.hpp>
#include <math.h>

class Quaternion;

namespace Maths
{
	constexpr double PI = 3.14159265358979323846;
	constexpr float PI_F = (float)PI;

	constexpr double SQRT2 = 1.4142135623730950488;
	constexpr float SQRT2_F = (float)SQRT2;

	constexpr double E = 2.71828182845904523536;
	constexpr float E_F = (float)E;

	constexpr const double DEGS_PER_RAD = 180.0 / PI;
	constexpr const double RADS_PER_DEG = PI / 180.0;

	constexpr const float DEGS_PER_RAD_F = 180.f / PI_F;
	constexpr const float RADS_PER_DEG_F = PI_F / 180.f;

	template <typename T>
	T Abs(const T& value) { return value < 0 ? -value : value; }

	template <typename T>
	T Lerp(const T& from, const T& to, float alpha) { return (T)((from * (1.f - alpha)) + (to * alpha)); }

	template <typename T>
	inline const T& Min(const T* elements, size_t size)
	{
		Debug::Assert(size > 0, "Maths::Min called with 0 size");

		const T* min = &elements[0];
		for (size_t i = 1; i < size; ++i)
			if (elements[i] < *min)
				min = &elements[i];

		return *min;
	}

	template <typename T>
	inline const T& Max(const T* elements, size_t size)
	{
		Debug::Assert(size > 0, "Maths::Max called with 0 size");

		const T* max = &elements[0];
		for (size_t i = 1; i < size; ++i)
			if (elements[i] > *max)
				max = &elements[i];

		return *max;
	}

	template <typename T>
	inline T Min(const T& a, const T& b) { return a < b ? a : b; }

	template <typename T>
	inline T Max(const T& a, const T& b) { return a > b ? a : b; }

	template <typename T>
	inline T Clamp(const T& x, const T& lower, const T& upper)
	{
		if (x < lower)
			return lower;
		if (x > upper)
			return upper;

		return x;
	}

	template<>
	Quaternion Lerp(const Quaternion& from, const Quaternion& to, float alpha);

	inline float Round(float x) 
	{
		float trunc = (float)(int32)x;
		float dec = x - trunc;

		if (dec >= .5f) return trunc + 1.f;
		else if (dec <= -.5f) return trunc - 1.f;
		return trunc;
	}

	inline float Round(float x, float nearest)
	{
		float inv = 1.f / nearest;
		return Round(x * inv) / inv;
	}

	inline float Floor(float x)
	{
		return floorf(x);
	}

	inline float Floor(float x, float nearest)
	{
		float inv = 1.f / nearest;
		return Floor(x * inv) / inv;
	}

	inline int Trunc(float x)
	{
		return (int)x;
	}

	inline float Trunc(float x, float nearest)
	{
		float inv = 1.f / nearest;
		return Trunc(x * inv) / inv;
	}

	float SquareRoot(float x);

	template<typename T>
	inline T Pow(T base, unsigned int index)
	{
		T result = (T)1;
		while (index--) result *= base;
		return result;
	}

	float Exp(float x);
	double Exp(double x);

	template<typename T>
	inline T RadiansToDegrees(T radians) { return (T)DEGS_PER_RAD * radians; }

	template<typename T>
	inline T DegreesToRadians(T degrees) { return (T)RADS_PER_DEG * degrees; }

	float Sin(float radians);
	double Sin(double radians);
	float Cos(float radians);
	double Cos(double radians);
	float Tan(float radians);
	double Tan(double radians);
	
	float ArcSin(float x);
	double ArcSin(double x);
	float ArcCos(float x);
	double ArcCos(double x);
	float ArcTan(float x);
	double ArcTan(double x);
	float ArcTan2(float y, float x);
	double ArcTan2(double y, double x);

	inline float SmallestAngularDiff(float to, float from)
	{
		if (to == from) return 0.f;

		float diff = to - from;
		while (diff > 180.f) diff -= 360.f;
		while (diff < -180.f) diff += 360.f;
		return diff;
	}

	template<typename T>
	inline bool AlmostEquals(const T& a, const T& b, const T& tolerance)
	{
		return (a >= b - tolerance && a <= b + tolerance);
	}

	int FloorPow2(int x);
}
