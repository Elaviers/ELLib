#pragma once
#include <ELGraphics/Colour.hpp>

class RenderEntry;

class UIColour
{
	bool _isBlend;

	union
	{
		Colour _colour;
		Colour _primary;
	};

	Colour _secondary;
	Colour _tertiary;

public:
	UIColour(const Colour& colour = Colour::White) : _colour(colour), _isBlend(false) {}
	UIColour(const Colour& primary, const Colour& secondary) : _primary(primary), _secondary(secondary), _tertiary(primary), _isBlend(true) {}
	UIColour(const Colour& primary, const Colour& secondary, const Colour& tertiary) : _primary(primary), _secondary(secondary), _tertiary(tertiary), _isBlend(true) {}
	~UIColour() {}

	UIColour(const UIColour& other) : _isBlend(other._isBlend), _primary(other._primary), _secondary(other._secondary), _tertiary(other._tertiary) {}

	UIColour& operator=(const UIColour& other)
	{
		_isBlend = other._isBlend;
		_primary = other._primary;
		_secondary = other._secondary;
		_tertiary = other._tertiary;
		
		return *this;
	}

	void Apply(RenderEntry&) const;
};
