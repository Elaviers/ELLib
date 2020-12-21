#include "Colour.hpp"

const Colour Colour::Invisible	(0.f, 0.f, 0.f, 0.f);
const Colour Colour::White		(1.f, 1.f, 1.f);
const Colour Colour::Grey		(.5f, .5f, .5f);
const Colour Colour::Black		(0.f, 0.f, 0.f);
const Colour Colour::Red		(1.f, 0.f, 0.f);
const Colour Colour::Green		(0.f, 1.f, 0.f);
const Colour Colour::Blue		(0.f, 0.f, 1.f);
const Colour Colour::Yellow		(1.f, 1.f, 0.f);
const Colour Colour::Pink		(1.f, 0.f, 1.f);
const Colour Colour::Cyan		(0.f, 1.f, 1.f);

String Colour::ToColourCode() const
{
	String string(0x01);

	for (int i = 0; i < 4; ++i)
		string += 1 + (byte)(_data[i] * 254.f);

	return string;
}

//static
Colour Colour::FromColourCode(const char* code)
{
	++code; //skip 0x01

	Colour colour;

	for (int i = 0; i < 4; ++i)
		colour._data[i] = ((float)(byte)code[i] - 1.f) / 254.f;

	return colour;
}
