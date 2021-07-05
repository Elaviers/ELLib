#include "Label.hpp"
#include <ELGraphics/RenderQueue.hpp>
#define CLIPPING_DEBUG FALSE

/*
	Setting LABEL_IMGUI_DEBUG_PATH will display an imgui window for each label
	which allows control of offsets/rotation (mostly useful for debugging text clipping)
*/
#define LABEL_IMGUI_DEBUG_PATH "../../Engine/imgui/imgui.h"
#undef LABEL_IMGUI_DEBUG_PATH

void UILabel::_UpdateShadowTransform()
{
	_shadowTransform = _transform;
	_shadowTransform.Move(Vector3(_shadowOffset.x, _shadowOffset.y, 0.f));

	_shadowClip.w = _clip.w;
	_shadowClip.h = _clip.h;
	_shadowClip.x = _clip.x - _shadowOffset.x;
	_shadowClip.y = _clip.y - _shadowOffset.y;
}

void UILabel::UpdateBounds()
{
	UIElement::UpdateBounds();

	if (_font)
	{
		float availableW = _absoluteBounds.w - _margin * 2.f;
		float availableH = _absoluteBounds.h - _margin * 2.f;
		float fontSize = availableH;
		float w = _font->CalculateStringWidth(_text.ToString().begin(), fontSize);

		/* behaviour disabled, prefer clipping to scaling text
		if (w > availableW)
		{
			float scale = availableW / w;
			fontSize *= scale;
			w *= scale;
			w = _font->CalculateStringWidth(_text.ToString().GetData(), fontSize);
		}
		*/

		float x;
		switch (_alignment)
		{
		case ETextAlignment::LEFT:
			x = _absoluteBounds.x + _offset.x + _margin;
			break;
		case ETextAlignment::RIGHT:
			x = _absoluteBounds.x + _offset.x + _absoluteBounds.w - w - _margin;
			break;
		case ETextAlignment::CENTRE:
		default:
			x = _absoluteBounds.x + _offset.x + _absoluteBounds.w / 2.f - w / 2.f;
			break;
		}

		_transform.SetPosition(Vector3(x, _absoluteBounds.y + _offset.y + _margin, _z));
		_transform.SetScale(Vector3(fontSize, fontSize, 1.f));

		_clip.x = _absoluteBounds.x + _margin - x;
		_clip.y = -_offset.y - _margin;
		_clip.w = _absoluteBounds.w - _margin * 2;
		_clip.h = _absoluteBounds.h - _margin * 2;
	}

	_UpdateShadowTransform();
}

#ifdef LABEL_IMGUI_DEBUG_PATH
	#include LABEL_IMGUI_DEBUG_PATH

	void UILabel::Update(float deltaTime)
	{
		if (_text.ToString().GetLength() == 0) return;

		float aoffset[2] = { _bounds.x.absolute, _bounds.y.absolute };
		float rot = _transform.GetRotation().GetEuler().z;

		ImGui::Begin(_text.ToString().GetData());
		ImGui::DragFloat2("Font Offset", &_offset[0], 1.f, -2000, 2000, "%.3fpx");
		ImGui::DragFloat2("Absolute Offsets", aoffset, 1.f, -2000, 2000, "%.3fpx");
		ImGui::SliderFloat("Rotation", &rot, -180.f, 180.f, "%.3f deg");
		ImGui::End();

		_bounds.x.absolute = aoffset[0];
		_bounds.y.absolute = aoffset[1];
		UpdateBounds();

		_transform.SetRotationEuler(Vector3(0.f, 0.f, rot));
		}
#else
void UILabel::Update(float deltaTime) {}
#endif

void UILabel::Render(RenderQueue& q) const
{
	if (_font)
	{
		RenderEntry& e = q.CreateEntry(ERenderChannels::UNLIT);
#if CLIPPING_DEBUG
		e.AddSetColour(Colour(.8f, .8f, .2f, .33f));
		e.AddSetTexture(RCMDSetTexture::Type::WHITE, 0);

		Vector3 tPos = Vector3((_clip.x + _clip.w / 2.f) / _transform.GetScale()[0], (_clip.y + _clip.h / 2.f) / _transform.GetScale()[1], 0.f);
		Vector3 tSz = Vector3(_clip.w, _clip.h, 1.f);

		tPos = (Vector4(tPos) * _transform.GetMatrix()).GetXYZ();

		e.AddSetTransform(Matrix4::Transformation(tPos, _transform.GetRotation().GetEuler(), tSz));
		e.AddCommand(RCMDRenderMesh::PLANE);
#endif

		if (_shadowOffset.x != 0.f && _shadowOffset.y != 0.f)
		{
			_shadowColour.Apply(e);
			_font->RenderString(e, _text.ToString().begin(), _shadowTransform, 0.f, _shadowClip);
		}

		_colour.Apply(e);
		_font->RenderString(e, _text.ToString().begin(), _transform, 0.f, _clip);
	}
}
