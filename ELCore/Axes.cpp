#include "Axes.hpp"

EAxis _horizontalAxes[3] = { EAxis::Z, EAxis::X, EAxis::X};
EAxis _verticalAxes[3] = { EAxis::Y, EAxis::Z, EAxis::Y};


//Returns the axis to horizontal to input, assuming simplest rotation
EAxis Axes::GetHorizontalAxis(EAxis axis) { return _horizontalAxes[(int)axis]; }

//Returns the axis vertical to input, assuming simplest rotation
EAxis Axes::GetVerticalAxis(EAxis axis) { return _verticalAxes[(int)axis]; }
