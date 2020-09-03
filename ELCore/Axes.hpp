#pragma once
#undef X
#undef Y
#undef Z

//Really a namespace.. oh well.
class Axes
{
public:
	enum EAxis { X = 0, Y = 1, Z = 2 };

private:
	static EAxis _horizontalAxes[3];
	static EAxis _verticalAxes[3];

public:
	Axes() = delete;
	~Axes() = delete;

	//Returns the axis to horizontal to input, assuming simplest rotation
	static int GetHorizontalAxis(EAxis axis) { return _horizontalAxes[(int)axis]; }

	//Returns the axis vertical to input, assuming simplest rotation
	static int GetVerticalAxis(EAxis axis) { return _verticalAxes[(int)axis]; }
};
