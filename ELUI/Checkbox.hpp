#pragma once
#include "Button.hpp"
#include "Rect.hpp"

class UICheckbox : public UIElement
{
	bool _value;

	bool _readOnly;

	UIButton _button;
	UIRect _image;

	SharedPointer<const Texture> _textureFalse;
	SharedPointer<const Texture> _textureTrue;

	UIColour _colourFalse;
	UIColour _colourTrue;

	void _OnButtonPressed(UIButton&) { Toggle(); };

public:
	Event<UICheckbox&> onStateChanged;

	UICheckbox(UIElement* parent = nullptr) : UIElement(parent), _button(this), _image(this), _value(false), _readOnly(false)
	{
		_button.onPressed += Function(&UICheckbox::_OnButtonPressed, *this);
	}

	virtual ~UICheckbox() {}

	bool GetState() const { return _value; }
	bool GetReadOnly() const { return _readOnly; }
	const SharedPointer<const Texture>& GetTextureFalse() const { return _textureFalse; }
	const SharedPointer<const Texture>& GetTextureTrue() const { return _textureTrue; }
	const UIColour& GetColourFalse() const { return _colourFalse; }
	const UIColour& GetColourTrue() const { return _colourTrue; }
	const UIColour& GetColourHover() const { return _button.GetColourHover(); }
	const UIColour& GetColourHold() const { return _button.GetColourHold(); }

	float GetBorderSize() const { return _button.GetBorderSize(); }
	const SharedPointer<const Material>& GetMaterial() const { return _button.GetMaterial(); }

	UICheckbox& SetState(bool state);
	UICheckbox& SetReadOnly(bool readOnly) { _readOnly = readOnly; return *this; }
	UICheckbox& SetTextureFalse(const SharedPointer<const Texture>& texture)
	{ 
		_textureFalse = texture;
		if (!_value) _image.SetTexture(texture);
		return *this;
	}
	UICheckbox& SetTextureTrue(const SharedPointer<const Texture>& texture)
	{ 
		_textureTrue = texture; 
		if (_value) _image.SetTexture(texture);
		return *this;
	}
	UICheckbox& SetColourFalse(const UIColour& colour) 
	{ 
		_colourFalse = colour; 
		if (!_value) _button.SetColour(colour);
		return *this; 
	}
	UICheckbox& SetColourTrue(const UIColour& colour) 
	{ 
		_colourTrue = colour; 
		if (_value) _button.SetColour(colour);
		return *this;
	}
	UICheckbox& SetColourHover(const UIColour& colour) { _button.SetColourHover(colour); return *this; }
	UICheckbox& SetColourHold(const UIColour& colour) { _button.SetColourHold(colour); return *this; }
	UICheckbox& SetMaterial(const SharedPointer<const Material>& material) { _button.SetMaterial(material); return *this; }
	UICheckbox& SetBorderSize(float borderSize) { _button.SetBorderSize(borderSize); return *this; }

	virtual void UpdateBounds() override
	{
		UIElement::UpdateBounds();
		_button.UpdateBounds();
		_image.UpdateBounds();
	}

	virtual void Render(RenderQueue& q) const override
	{
		_button.Render(q);

		if ((_value && _textureTrue) || (!_value && _textureFalse))
			_image.Render(q);
	}

	virtual bool OnKeyDown(bool blocked, EKeycode key) override { return _readOnly ? false : _button.OnKeyDown(blocked, key); }
	virtual bool OnKeyUp(bool blocked, EKeycode key) override  { return _readOnly ? false : _button.OnKeyUp(blocked, key); }
	virtual bool OnMouseMove(bool blocked, float x, float y) override { return _readOnly ? false : _button.OnMouseMove(blocked, x, y); }

	void Toggle()
	{
		SetState(!_value);
	}
};
