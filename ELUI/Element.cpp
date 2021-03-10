#include "Element.hpp"
#include "Container.hpp"

UIElement::UIElement(UIElement* parent) : _parent(parent), _markedForDelete(false), _hasFocus(false), _focusOnClick(true), _hover(false), _z(0.f), _cursor(ECursor::DEFAULT)
{
	onFocusGained += Function<void, UIElement&>(*this, &UIElement::_SetFocus);
	onFocusLost += Function<void, UIElement&>(*this, &UIElement::_ClearFocus);

	if (_parent)
	{
		UpdateBounds();
		_parent->_OnChildGained(this);
	}
}

UIElement::~UIElement()
{
	if (_parent)
		_parent->_OnChildLost(this);
}

UIElement& UIElement::SetParent(UIElement* parent)
{
	if (_parent)
		_parent->_OnChildLost(this);

	_parent = parent;
	UpdateBounds();

	if (_parent)
		_parent->_OnChildGained(this);

	return *this;
}

UIElement& UIElement::SetZ(float z)
{
	_z = z; 
	UpdateBounds(); 
	
	UIContainer* p = dynamic_cast<UIContainer*>(_parent);
	if (p) p->_SortChildren();

	return *this;
}

void UIElement::RequestFocus()
{
	if (_parent)
	{
		UIElement* root = _parent;
		while (root->_parent)
			root = root->_parent;

		root->FocusElement(this);
	}
}

void UIElement::FocusElement(UIElement* element)
{
	if (element == this)
		onFocusGained(*this);
	else
		onFocusLost(*this);
}

void UIElement::UpdateBounds()
{
	if (_parent)
	{
		_absoluteBounds.x = _parent->_clientBounds.x + (_parent->_clientBounds.w * _bounds.x.relative) + _bounds.x.absolute;
		_absoluteBounds.y = _parent->_clientBounds.y + (_parent->_clientBounds.h * _bounds.y.relative) + _bounds.y.absolute;
		_absoluteBounds.w = _parent->_clientBounds.w * _bounds.w.relative + _bounds.w.absolute;
		_absoluteBounds.h = _parent->_clientBounds.h * _bounds.h.relative + _bounds.h.absolute;
	}
	else
	{
		_absoluteBounds.x = _bounds.x.relative + _bounds.x.absolute;
		_absoluteBounds.y = _bounds.y.relative + _bounds.y.absolute;
		_absoluteBounds.w = _bounds.w.relative + _bounds.w.absolute;
		_absoluteBounds.h = _bounds.h.relative + _bounds.h.absolute;
	}

	_UpdateClientBounds();
}

void UIElement::_UpdateClientBounds()
{
	_clientBounds = _absoluteBounds;
}

bool UIElement::OnKeyDown(bool blocked, EKeycode key)
{
	if (key == EKeycode::MOUSE_LEFT && _focusOnClick && _hover)
	{
		RequestFocus();
		return true;
	}

	return false;
}

bool UIElement::OnKeyUp(bool blocked, EKeycode key)
{
	return key == EKeycode::MOUSE_LEFT && _focusOnClick && _hover;
}

bool UIElement::OnMouseMove(bool blocked, float x, float y)
{
	if (blocked)
	{
		if (_hover)
		{
			_hover = false;
			OnHoverStop();
		}

		return false;
	}

	bool hover = OverlapsPoint(x, y);
	if (hover != _hover)
	{
		_hover = hover;
		_hover ? OnHoverStart() : OnHoverStop();
	}

	return _hover;
}
