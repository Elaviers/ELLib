#include "ComboBox.hpp"

void UIComboBox::_StopSelecting()
{
	_selecting = false;

	_hover = true; //messy, this goes back from being hover for the whole element to just the hover for the button
	UITextButton::OnMouseMove(_lastMouseX, _lastMouseY, true);
}

void UIComboBox::_OnPressed(UIButton& button)
{
	RequestFocus();

	_panel.SetColour(_colourHold);
	_selecting = !_selecting;
}

void UIComboBox::_OnSelectionSubmitted(UILabel& label)
{
	SetString(label.GetString());
	_StopSelecting();

	onStringChanged(*this);
}

void UIComboBox::_UpdateListBounds()
{
	float h = _listBox.GetItemHeight() * _listBox.GetItems().GetSize();
	_listBox.SetBounds(0.f, UICoord(1.f, -h - _absoluteBounds.h), 1.f, UICoord(0.f, h));
}

bool UIComboBox::OnMouseMove(bool blocked, float x, float y)
{
	if (_readOnly)
		return false;

	bool b1 = false;
	if (!_selecting)
		b1 = UITextButton::OnMouseMove(blocked, x, y);
	else
		b1 = UITextButton::OverlapsPoint(x, y);

	float b2 = _listBox.OnMouseMove(blocked, x, y);

	_lastMouseX = x;
	_lastMouseY = y;

	if (_selecting)
	{
		_hover = b1 || b2;
		return _hover;
	}
	
	return b1 || b2;
}

bool UIComboBox::OnKeyDown(bool blocked, EKeycode key)
{
	if (UITextButton::OnKeyDown(blocked, key)) return true;
	return _listBox.OnKeyDown(blocked, key);
}

bool UIComboBox::OnKeyUp(bool blocked, EKeycode key)
{
	if (_readOnly)
		return false;

	if (_selecting)
	{
		if (_listBox.OverlapsPoint(_lastMouseX, _lastMouseY))
			return _listBox.OnKeyUp(blocked, key);
		else
			_StopSelecting();
	}
	else
		return UITextButton::OnKeyUp(blocked, key);

	return false;
}
