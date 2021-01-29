#include "Maths.hpp"
#include "Quaternion.hpp"
#include <stdlib.h>
#include <math.h>

namespace Maths
{
	float RoundOff(float x)
	{
		if (x <= 0.0001f && x >= -0.0001f)
			x = 0.f;

		return x;
	}

	float SquareRoot(float x)
	{
		return sqrtf(x);
	}

	template <>
	Quaternion Lerp(const Quaternion& from, const Quaternion& to, float alpha) { return Quaternion::Lerp(from, to, alpha); }

	double Sin(double radians)
	{
		return sin(radians);
	}

	double Cos(double radians)
	{
		return cos(radians);
	}

	double Tan(double radians)
	{
		return tan(radians);
	}

	double ArcSin(double x)
	{
		return asin(x);
	}

	double ArcCos(double x)
	{
		return asin(x);
	}

	double ArcTan(double x)
	{
		return atan(x);
	}

	float Sin(float radians)
	{
		return sinf(radians);
	}

	float Cos(float radians)
	{
		return cosf(radians);
	}

	float Tan(float radians)
	{
		return tanf(radians);
	}

	float ArcSin(float x)
	{
		return asinf(x);
	}

	float ArcCos(float x)
	{
		return asinf(x);
	}

	float ArcTan(float x)
	{
		return atanf(x);
	}

	void SetRandomSeed(unsigned int seed)
	{
		srand(seed);
	}

	float ArcTan2(float y, float x)
	{
		if (x > 0.f) return ArcTan(y / x);
		if (x < 0.f) return ArcTan(y / x) + ((y < 0.f) ? -PI_F : PI_F);
		if (y == 0.f) return 0.f;
		return y < 0.f ? (-PI_F / 2.f) : (PI_F / 2.f);
	}

	double ArcTan2(double y, double x)
	{
		if (x > 0.0) return ArcTan(y / x);
		if (x < 0.0) return ArcTan(y / x) + ((y < 0.0) ? -PI : PI);
		if (y == 0.0) return 0.0;
		return y < 0.0 ? (-PI / 2.0) : (PI / 2.0);
	}

	int FloorPow2(int x)
	{
		x |= x >> 1;
		x |= x >> 2;
		x |= x >> 4;
		x |= x >> 8;
		x |= x >> 16;
//#if sizeof(int) >= 8
//		x |= x >> 32;
//#endif
		return x - (x >> 1);
	}
}
