#include "Button.hpp"

void UIButton::OnHoverStart()
{
	_panel.SetColour(_colourHover);
}

void UIButton::OnHoverStop()
{
	_panel.SetColour(_colour);
	_hold = false;
}

bool UIButton::OnKeyDown(bool blocked, EKeycode key)
{
	if (key == EKeycode::MOUSE_LEFT && _hover)
	{
		_hold = true;
		_panel.SetColour(_colourHold);
		return true;
	}

	return false;
}

bool UIButton::OnKeyUp(bool blocked, EKeycode key)
{
	if (key == EKeycode::MOUSE_LEFT && _hold)
	{
		_hold = false;
		onPressed(*this);

		_panel.SetColour(_colourHover);

		return true;
	}

	return false;
}
