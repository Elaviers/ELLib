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

bool UIComboBox::OnMouseMove(float mouseX, float mouseY, bool blocked)
{
	if (_readOnly)
		return false;

	bool b1 = false;
	if (!_selecting)
		b1 = UITextButton::OnMouseMove(mouseX, mouseY, blocked);
	else
		b1 = UITextButton::OverlapsPoint(mouseX, mouseY);

	float b2 = _listBox.OnMouseMove(mouseX, mouseY, blocked);

	_lastMouseX = mouseX;
	_lastMouseY = mouseY;

	if (_selecting)
	{
		_hover = b1 || b2;
		return _hover;
	}
	
	return b1 || b2;
}

bool UIComboBox::OnMouseDown()
{
	if (UITextButton::OnMouseDown()) return true;
	return _listBox.OnMouseDown();
}

bool UIComboBox::OnMouseUp()
{
	if (_readOnly)
		return false;

	if (_selecting)
	{
		if (_listBox.OverlapsPoint(_lastMouseX, _lastMouseY))
			return _listBox.OnMouseUp();
		else
			_StopSelecting();
	}
	else
		return UITextButton::OnMouseUp();

	return false;
}
