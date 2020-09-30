#include "Label.hpp"
#include <ELGraphics/RenderQueue.hpp>

void UILabel::_UpdateShadowTransform()
{
	_shadowTransform = _transform;
	_shadowTransform.Move(Vector3(_shadowOffset.x, _shadowOffset.y, 0.f));
}

void UILabel::Render(RenderQueue& q) const
{
	if (_font)
	{
		RenderEntry& e = q.CreateEntry(ERenderChannels::UNLIT);

		if (_shadowOffset.x != 0.f && _shadowOffset.y != 0.f)
		{
			_shadowColour.Apply(e);
			_font->RenderString(e, _string.GetData(), _shadowTransform);
		}

		_colour.Apply(e);
		_font->RenderString(e, _string.GetData(), _transform);
	}
}

void UILabel::_OnBoundsChanged()
{
	if (_font)
	{
		float availableW = _absoluteBounds.w - _margin * 2.f;
		float availableH = _absoluteBounds.h - _margin * 2.f;
		float w = _font->CalculateStringWidth(_string.GetData(), _absoluteBounds.h);

		float fontSize = availableH;
		if (w > availableW)
		{
			float scale = availableW / w;
			fontSize *= scale;
			w *= scale;
			w = _font->CalculateStringWidth(_string.GetData(), fontSize);

			//todo: String width scaling not linear with font size, I'll have to clip it to the textbox like a sane person
		}

		float x;
		switch (_alignment)
		{
		case ETextAlignment::LEFT: 
			x = _absoluteBounds.x + _margin;
			break;
		case ETextAlignment::RIGHT:
			x = _absoluteBounds.x + _absoluteBounds.w - w - _margin;
			break;
		case ETextAlignment::CENTRE:
		default:
			x = _absoluteBounds.x + _absoluteBounds.w / 2.f - w / 2.f;
			break;
		}

		_transform.SetPosition(Vector3(x, _absoluteBounds.y + _margin, _z));
		_transform.SetScale(Vector3(fontSize, fontSize, 1.f));
	}

	_UpdateShadowTransform();
}
