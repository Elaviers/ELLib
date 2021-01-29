#pragma once
#include "Types.hpp"

//IEEE754 binary32 with radix of 2, little-endian
struct Float_IEEE754
{
	uint32 mantissa : 23;
	uint32 exponent : 8;
	uint32 sign : 1;

	Float_IEEE754(byte sign, byte exponent, uint32 mantissa) : mantissa(mantissa), exponent(exponent), sign(sign) {}
	Float_IEEE754() : Float_IEEE754(0,0,0) {}

	static const Float_IEEE754 Zero;
	static const Float_IEEE754 Infinity;
	static const Float_IEEE754 NegInfinity;
	static const Float_IEEE754 NaN;

	static Float_IEEE754 FromFloat(float value);
	float ToFloat() const;

	static Float_IEEE754 FromFloat_ForceConversion(float value);
	float ToFloat_ForceConversion() const;
};

union Float_IEEE754_U
{
	Float_IEEE754 binary32;
	float fVal;
	uint32 raw;
	byte bytes[4];

	Float_IEEE754_U() : raw(0) {}
	Float_IEEE754_U(const Float_IEEE754& binary32) : binary32(binary32) {}
};
