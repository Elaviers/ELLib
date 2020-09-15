#include "Colour.hpp"
#include <ELGraphics/RenderCommand.hpp>
#include <ELGraphics/RenderEntry.hpp>

void UIColour::Apply(RenderEntry& e) const
{
	if (_isBlend)
		e.AddSetColour(_primary, _secondary, _tertiary);
	else
		e.AddSetColour(_colour);
}
