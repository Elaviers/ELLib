#pragma once
#include "Button.hpp"

class UITextButton : public UIButton
{
protected:
	UILabel _label;

public:
	UITextButton(UIElement* parent = nullptr) : UIButton(parent), _label(this) { }
	virtual ~UITextButton() {}

	const UIColour& GetTextColour() const { return _label.GetColour(); }
	const UIColour& GetTextShadowColour() const { return _label.GetShadowColour(); }
	const Vector2& GetTextShadowOffset() const { return _label.GetShadowOffset(); }
	ETextAlignment GetTextAlignment() const { return _label.GetAlignment(); }
	float GetTextMargin() const { return _label.GetMargin(); }
	const SharedPointer<const Font>& GetFont() const { return _label.GetFont(); }
	const Text& GetText() const { return _label.GetText(); }

	UITextButton& SetTextColour(const UIColour& colour) { _label.SetColour(colour); return *this; }
	UITextButton& SetTextShadowColour(const UIColour& colour) { _label.SetShadowColour(colour); return *this; }
	UITextButton& SetTextShadowOffset(const Vector2& offset) { _label.SetShadowOffset(offset); return *this; }
	UITextButton& SetTextAlignment(ETextAlignment alignment) { _label.SetAlignment(alignment); return *this; }
	UITextButton& SetTextMargin(float margin) { _label.SetMargin(margin); return *this; }
	UITextButton& SetFont(const SharedPointer<const Font>& font) { _label.SetFont(font); return *this; }
	UITextButton& SetText(const Text& text) { _label.SetText(text); return *this; }

	virtual void UpdateBounds() override
	{
		UIButton::UpdateBounds();
		_label.UpdateBounds();
	}

	virtual void Render(RenderQueue& q) const override
	{
		UIButton::Render(q);
		_label.Render(q);
	}
};
