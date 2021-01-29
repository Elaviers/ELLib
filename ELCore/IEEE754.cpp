#include "IEEE754.hpp"
#include <cmath>

const Float_IEEE754 Float_IEEE754::Zero = Float_IEEE754(0,0,0);
const Float_IEEE754 Float_IEEE754::Infinity = Float_IEEE754(0, 0xFF, 0);
const Float_IEEE754 Float_IEEE754::NegInfinity = Float_IEEE754(1, 0xFF, 0);
const Float_IEEE754 Float_IEEE754::NaN = Float_IEEE754(0, 0xFF, 0x7FFFFF);

const union
{
	float f = 172172.03125f;
	uint32 d;
} _t1;

const bool _nativeIEEE = _t1.d == 0xC8282302;

__forceinline void _Float2BIN32(Float_IEEE754& binary32, float value)
{
	if (value < 0)
	{
		binary32.sign = 1;
		value *= -1.f;
	}

	signed int shift = 0;
	if (value < 1.f)
	{
		//left shift until the whole part is 1
		for (shift = -1; shift > -127; --shift)
		{
			value *= 2;

			if ((uint32)value > 0.f)
				break;
		}
	}
	else
	{
		//right shift until the whole part is about to be 0
		float nextValue;
		for (; shift < 127; ++shift)
		{
			nextValue = value / 2.f;

			if ((uint32)nextValue == 0.f)
				break;

			value = nextValue;
		}
	}

	//value is now normalised (1.b0b1b2b3...).. we're using it to find the mantissa so the whole part can be omitted
	binary32.mantissa = (uint32)((value - (uint32)value) * 0x800000); //leftshifted by 23 bits
	binary32.exponent = 127 + shift;
}

__forceinline float _BIN322Float(const Float_IEEE754& binary32)
{
	float val = 1.f + ((float)binary32.mantissa / (float)0x800000);

	if (binary32.exponent > 127)
		for (int i = binary32.exponent; i > 127; --i)
			val *= 2.f;
	else
		for (int i = binary32.exponent; i < 127; ++i)
			val /= 2.f;

	return binary32.sign ? -val : val;
}

Float_IEEE754 Float_IEEE754::FromFloat(float value)
{
	if (std::isnan(value)) return NaN;
	if (value == 0.f) return Zero;
	if (value == INFINITY) return Infinity;
	if (value == -INFINITY) return NegInfinity;

	if (_nativeIEEE) return *reinterpret_cast<const Float_IEEE754*>(&value);

	Float_IEEE754 binary32;
	_Float2BIN32(binary32, value);
	return binary32;
}

Float_IEEE754 Float_IEEE754::FromFloat_ForceConversion(float value)
{
	if (std::isnan(value)) return NaN;
	if (value == 0.f) return Zero;
	if (value == INFINITY) return Infinity;
	if (value == -INFINITY) return NegInfinity;

	Float_IEEE754 binary32;
	_Float2BIN32(binary32, value);
	return binary32;
}

float Float_IEEE754::ToFloat() const
{
	if (exponent == 0) return 0.f;
	if (exponent == 255)
	{
		if (mantissa == 0) return sign > 0 ? -INFINITY : INFINITY;
		return NAN;
	}

	if (_nativeIEEE) return *reinterpret_cast<const float*>(this);

	return _BIN322Float(*this);
}

float Float_IEEE754::ToFloat_ForceConversion() const
{
	if (exponent == 0) return 0.f;
	if (exponent == 255)
	{
		if (mantissa == 0) return sign > 0 ? -INFINITY : INFINITY;
		return NAN;
	}

	return _BIN322Float(*this);
}
