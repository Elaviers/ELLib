#include "Textbox.hpp"
#include <ELGraphics/RenderCommand.hpp>
#include <ELGraphics/RenderQueue.hpp>

void _CalculateTextStartStop(const AbsoluteBounds& bounds, const Transform& renderTransform, float margin, float& start, float& stop)
{
	Vector3 rv = renderTransform.GetRightVector();
	Vector3 uv = renderTransform.GetUpVector();

	Vector3 vstart = Vector3(bounds.x, bounds.y, 0.f) + rv * margin + uv * margin;
	Vector3 vend = vstart + rv * (bounds.w - margin * 2.f);

	start = (vstart - renderTransform.GetPosition()).Length();
	stop = (vend - renderTransform.GetPosition()).Length();
}

void UITextbox::_ResetCaretBlink()
{
	_caretTimer = _caretPeriod;
	_caretStatus = true;
}

void UITextbox::_UpdateSelectionBox()
{
	_selectionTransform = _label.GetRenderTransform();

	float startX = _selectionStart == 0 ? 0 : _label.GetFont()->CalculateStringWidth(_label.GetText().ToString().begin(), _selectionTransform.GetScale().x, _selectionStart);
	float endX = _selectionEnd == 0 ? 0 : _label.GetFont()->CalculateStringWidth(_label.GetText().ToString().begin(), _selectionTransform.GetScale().x, _selectionEnd);

	if (startX > endX) Utilities::Swap(startX, endX);

	float pstart, pstop;
	_CalculateTextStartStop(_label.GetAbsoluteBounds(), _selectionTransform, _label.GetMargin(), pstart, pstop);

	startX = Maths::Max(startX, pstart);
	endX = Maths::Min(endX, pstop);

	_selectionTransform.SetScale(Vector3(endX - startX, _selectionTransform.GetScale().y, 0.f));
	_selectionTransform.Move(((endX + startX) / 2.f) * _selectionTransform.GetRightVector() + (_selectionTransform.GetScale().y / 2.f) * _selectionTransform.GetUpVector());
}

void UITextbox::_RemoveSelection()
{
	if (_selectionStart > _selectionEnd) Utilities::Swap(_selectionStart, _selectionEnd);

	SetText(Text(GetText().ToString().SubString(0, _selectionStart) + GetText().ToString().SubString(_selectionEnd)));

	SetCaretPosition(_selectionStart);
	_selectionStart = _selectionEnd = 0;
}

UITextbox& UITextbox::SetCaretPosition(size_t pos)
{
	_caretPos = pos;

	if (_label.GetFont())
	{
		_caretTransform = _label.GetRenderTransform();
		
		float pstart, pstop;
		_CalculateTextStartStop(_label.GetAbsoluteBounds(), _caretTransform, _label.GetMargin(), pstart, pstop);

		const Font* font = _label.GetFont().Ptr();
		const char* str = _label.GetText().ToString().begin();
		float pcaret = _caretOffset * _caretTransform.GetScale().x + _caretPos == 0 ? 0 : font->CalculateStringWidth(str, _caretTransform.GetScale().x, _caretPos);

		float width = pstop - pstart - _label.GetMargin();

		if (pcaret < pstart)
			_label.SetTextOffset(Vector2(-pcaret, 0.f));
		else if (pcaret > pstop)
			_label.SetTextOffset(Vector2(width - pcaret, 0.f));
		
		_caretTransform = _label.GetRenderTransform();
		float caretX = _caretOffset * _caretTransform.GetScale().x +
			(_caretPos == 0 ?
				0.f :
				font->CalculateStringWidth(str, _caretTransform.GetScale().x, _caretPos));

		_caretTransform.Move(_caretTransform.GetRightVector() * caretX + _caretTransform.GetUpVector() * (_caretTransform.GetScale().y / 2.f));
		_caretTransform.SetScale(Vector3(_caretWidth, _caretTransform.GetScale().y, 0.f));
	}

	return *this;
}

void UITextbox::UpdateBounds()
{
	UIElement::UpdateBounds();
	_label.UpdateBounds();
	_panel.UpdateBounds();
	SetCaretPosition(_caretPos);
	_UpdateSelectionBox();
}

void UITextbox::Render(RenderQueue& q) const
{
	_panel.Render(q);
	_label.Render(q);
	
	if (_selectionStart != _selectionEnd)
	{
		RenderEntry& e = q.CreateEntry(ERenderChannels::UNLIT, 0);
		e.AddSetTransform(_selectionTransform.GetMatrix());
		e.AddSetTexture(RCMDSetTexture::Type::WHITE, 0);
		e.AddSetColour(_selectionColour);
		e.AddCommand(RCMDRenderMesh::PLANE);
	}

	if (_hasFocus && _caretStatus)
	{
		RenderEntry& e = q.CreateEntry(ERenderChannels::UNLIT, 0);
		_caretColour.Apply(e);
		e.AddSetTransform(_caretTransform.GetMatrix());
		e.AddSetTexture(RCMDSetTexture::Type::WHITE, 0);
		e.AddCommand(RCMDRenderMesh::PLANE);
	}
}

void UITextbox::Update(float deltaTime)
{
	_label.Update(deltaTime);

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

			SetCaretPosition(_label.GetFont()->GetPositionOf(_lastMouseX, _lastMouseY, _label.GetText().ToString().begin(), _label.GetRenderTransform()));

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
			if (_caretPos > 0) SetCaretPosition(_caretPos - 1);

			_selecting = false;
			_selectionStart = _selectionEnd = 0;
			_ResetCaretBlink();
			break;
		
		case EKeycode::RIGHT:
			if (_caretPos < GetText().ToString().GetLength()) SetCaretPosition(_caretPos + 1);

			_selecting = false;
			_selectionStart = _selectionEnd = 0;
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
				SetCaretPosition(_caretPos - 1);

				_ResetCaretBlink();
			}

			onStringChanged(*this);
		}
		else if (c >= ' ')
		{
			if (_selectionStart != _selectionEnd)
				_RemoveSelection();

			SetText(Text(GetText().ToString().SubString(0, _caretPos) + c + GetText().ToString().SubString(_caretPos)));
			SetCaretPosition(_caretPos + 1);

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

	_lastMouseX = x;
	_lastMouseY = y;

	if (_selecting)
	{
		SetCaretPosition(_label.GetFont()->GetPositionOf(_lastMouseX, _lastMouseY, _label.GetText().ToString().begin(), _label.GetRenderTransform()));
		_selectionEnd = _caretPos;

		_UpdateSelectionBox();
	}

	return _hover;
}
