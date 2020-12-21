#include "Textbox.hpp"
#include <ELGraphics/RenderCommand.hpp>
#include <ELGraphics/RenderQueue.hpp>

void UITextbox::_ResetCaretBlink()
{
	_caretTimer = _caretPeriod;
	_caretStatus = true;
}

void UITextbox::_UpdateSelectionBox()
{
	Transform t = _label.GetRenderTransform();

	float startX = t.GetPosition().x + (_selectionStart == 0 ? 0 : _label.GetFont()->CalculateStringWidth(_label.GetText().ToString().GetData(), _label.GetRenderTransform().GetScale().x, _selectionStart));
	float endX = t.GetPosition().x + (_selectionEnd == 0 ? 0 : _label.GetFont()->CalculateStringWidth(_label.GetText().ToString().GetData(), _label.GetRenderTransform().GetScale().x, _selectionEnd));

	if (startX > endX) Utilities::Swap(startX, endX);

	_selectionRect.SetBounds(UIBounds(UICoord(0.f, startX - _absoluteBounds.x), 0.f, UICoord(0.f, endX - startX), 1.f));
}

void UITextbox::_RemoveSelection()
{
	if (_selectionStart > _selectionEnd) Utilities::Swap(_selectionStart, _selectionEnd);

	SetText(Text(GetText().ToString().SubString(0, _selectionStart) + GetText().ToString().SubString(_selectionEnd)));

	_caretPos = _selectionStart;
	_selectionStart = _selectionEnd = 0;
	_selectionRect.SetW(0.f);
}

void UITextbox::UpdateBounds()
{
	UIElement::UpdateBounds();
	_label.UpdateBounds();
	_panel.UpdateBounds();
	_selectionRect.UpdateBounds();
}

void UITextbox::Render(RenderQueue& q) const
{
	_panel.Render(q);
	_label.Render(q);
	_selectionRect.Render(q);

	if (_hasFocus && _caretStatus)
	{
		Transform t = _label.GetRenderTransform();
		float caretX = _caretOffset * t.GetScale().x + (_caretPos == 0 ? 0.f : _label.GetFont()->CalculateStringWidth(_label.GetText().ToString().GetData(), t.GetScale().x, _caretPos));
		t.Move(Vector3(caretX, t.GetScale().y / 2.f, 0.f));
		t.SetScale(Vector3(_caretWidth, t.GetScale().y, 0.f));

		RenderEntry& e = q.CreateEntry(ERenderChannels::UNLIT, 0);
		_caretColour.Apply(e);

		e.AddSetTransform(t.GetTransformationMatrix());
		e.AddSetTexture(RCMDSetTexture::Type::WHITE, 0);
		e.AddCommand(RCMDRenderMesh::PLANE);
	}
}

void UITextbox::Update(float deltaTime)
{
	_caretTimer -= deltaTime;

	if (_caretTimer <= 0.f)
	{
		_caretTimer = _caretPeriod;
		_caretStatus = !_caretStatus;
	}
}

bool UITextbox::OnKeyDown(bool blocked, EKeycode key)
{
	if (key == EKeycode::MOUSE_LEFT)
	{
		if (_hover)
		{
			RequestFocus();

			_caretPos = _label.GetFont()->GetPositionOf(_lastMouseX, _lastMouseY, _label.GetText().ToString().GetData(), _label.GetRenderTransform());

			_selecting = true;
			_selectionStart = _caretPos;
			_selectionEnd = _caretPos;

			_ResetCaretBlink();
			_UpdateSelectionBox();

			return true;
		}

		return false;
	}

	if (_hasFocus && !_readOnly)
	{
		switch (key)
		{
		case EKeycode::LEFT:
			if (_caretPos > 0) --_caretPos;

			_selecting = false;
			_selectionStart = _selectionEnd = 0;
			_selectionRect.SetW(0.f);
			_ResetCaretBlink();
			break;
		
		case EKeycode::RIGHT:
			if (_caretPos < GetText().ToString().GetLength()) ++_caretPos;

			_selecting = false;
			_selectionStart = _selectionEnd = 0;
			_selectionRect.SetW(0.f);
			_ResetCaretBlink();
			break;

		case EKeycode::ENTER:
			onStringSubmitted(*this);
			break;
		}

		return true;
	}

	return false;
}

bool UITextbox::OnKeyUp(bool blocked, EKeycode key)
{
	if (key == EKeycode::MOUSE_LEFT)
	{
		_selecting = false;
		return _hover;
	}

	return _hasFocus && !_readOnly;
}

bool UITextbox::OnCharInput(bool blocked, char c)
{
	if (_hasFocus && !_readOnly)
	{
		if (c == '\b')
		{
			if (_selectionStart != _selectionEnd)
			{
				_RemoveSelection();
			}
			else if (_caretPos > 0)
			{
				SetText(Text(GetText().ToString().SubString(0, _caretPos - 1) + GetText().ToString().SubString(_caretPos)));
				--_caretPos;

				_ResetCaretBlink();
			}

			onStringChanged(*this);
		}
		else if (c >= ' ')
		{
			if (_selectionStart != _selectionEnd)
				_RemoveSelection();

			SetText(Text(GetText().ToString().SubString(0, _caretPos) + c + GetText().ToString().SubString(_caretPos)));
			++_caretPos;

			_ResetCaretBlink();

			onStringChanged(*this);
		}

		return true;
	}

	return false;
}

bool UITextbox::OnMouseMove(bool blocked, float x, float y)
{
	UIElement::OnMouseMove(blocked, x, y);

	if (_hover)
	{
		_lastMouseX = x;
		_lastMouseY = y;
	}

	if (_selecting)
	{
		_caretPos = _label.GetFont()->GetPositionOf(_lastMouseX, _lastMouseY, _label.GetText().ToString().GetData(), _label.GetRenderTransform());
		_selectionEnd = _caretPos;

		_UpdateSelectionBox();
	}

	return _hover;
}
