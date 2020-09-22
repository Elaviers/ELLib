#pragma once
#undef X
#undef Y
#undef Z

enum class EAxis
{
	X = 0,
	Y = 1,
	Z = 2
};

namespace Axes
{
	//Returns the axis to horizontal to input, assuming simplest rotation
	EAxis GetHorizontalAxis(EAxis axis);

	//Returns the axis vertical to input, assuming simplest rotation
	EAxis GetVerticalAxis(EAxis axis);
};
