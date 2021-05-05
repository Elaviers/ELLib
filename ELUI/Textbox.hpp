#pragma once
#include "Label.hpp"
#include "Panel.hpp"
#include "Rect.hpp"

class UITextbox : public UIElement
{
private:
	float _lastMouseX = 0.f;
	float _lastMouseY = 0.f;

	bool _caretStatus = false;
	float _caretTimer = 0.f;
	size_t _caretPos = 0;

	size_t _selectionStart = 0;
	size_t _selectionEnd = 0;

protected:
	bool _selecting;

	bool _readOnly;

	UIPanel _panel;
	UILabel _label;

	Colour _selectionColour;
	Transform _selectionTransform;

	float _caretWidth;
	float _caretOffset;
	float _caretPeriod;
	UIColour _caretColour;

	Transform _caretTransform;

	void _OnFocusLost(UIElement& element) { _selectionStart = _selectionEnd = 0; }

	void _ResetCaretBlink();
	void _UpdateSelectionBox();
	void _RemoveSelection();

public:
	Event<UITextbox&> onStringChanged;
	Event<UITextbox&> onStringSubmitted;

	UITextbox(UIElement* parent = nullptr) : UIElement(parent),
		_panel(this), _label(this), _selectionStart(0), _selectionEnd(0), _selectionColour(Colour(0.f, .2f, 1.f, .2f)),
		_selecting(false), _readOnly(false),
		_caretWidth(2.f), _caretOffset(0.f), _caretPeriod(0.33f), _caretColour(Colour::Black)
	{
		SetCursor(ECursor::IBEAM);

		UIElement::onFocusLost += Function(&UITextbox::_OnFocusLost, *this);
	}

	virtual ~UITextbox() {}

	UITextbox& SetReadOnly(bool readOnly) { _readOnly = readOnly; return *this; }
	UITextbox& SetCaretColour(const UIColour& colour) { _caretColour = colour; return *this; }
	UITextbox& SetCaretPeriod(float caretPeriod) { _caretPeriod = caretPeriod; return *this; }
	UITextbox& SetCaretOffset(float offset) { _caretOffset = offset; SetCaretPosition(_caretPos); return *this; }
	UITextbox& SetCaretPosition(size_t pos);
	UITextbox& SetMaterial(const SharedPointer<const Material>& material) { _panel.SetMaterial(material); return *this; }
	UITextbox& SetColour(const UIColour& colour) { _panel.SetColour(colour);  return *this; }
	UITextbox& SetText(const Text& text) { _label.SetText(text); onStringChanged(*this); SetCaretPosition(_caretPos); return *this; }
	UITextbox& SetTextColour(const UIColour& colour) { _label.SetColour(colour);  return *this; }
	UITextbox& SetTextShadowColour(const UIColour& colour) { _label.SetShadowColour(colour); return *this; }
	UITextbox& SetTextShadowOffset(const Vector2& offset) { _label.SetShadowOffset(offset); return *this; }
	UITextbox& SetTextAlignment(ETextAlignment alignment) { _label.SetAlignment(alignment); SetCaretPosition(_caretPos); return *this; }
	UITextbox& SetFont(const SharedPointer<const Font>& font) { _label.SetFont(font); SetCaretPosition(_caretPos);  return *this; }
	UITextbox& SetBorderSize(float size) { _panel.SetBorderSize(size);  return *this; }
	UITextbox& SetSelectionColour(const Colour& colour) { _selectionColour = colour; return *this; }

	bool GetReadOnly() const { return _readOnly; }
	const UIColour& GetCaretColour() const { return _caretColour; }
	float GetCaretPeriod() const { return _caretPeriod; }
	float GetCaretOffset() const { return _caretOffset; }
	size_t GetCaretPosition() const { return _caretPos; }
	const SharedPointer<const Material>& GetMaterial() const { return _panel.GetMaterial(); }
	const UIColour& GetColour() const { return _panel.GetColour(); }
	const Text& GetText() const { return _label.GetText(); }
	const UIColour& GetTextColour() const { return _label.GetColour(); }
	const UIColour& GetTextShadowColour() const { return _label.GetShadowColour(); }
	const Vector2& GetTextShadowOffset() const { return _label.GetShadowOffset(); }
	ETextAlignment GetTextAlignment() const { return _label.GetAlignment(); }
	const SharedPointer<const Font>& GetFont() const { return _label.GetFont(); }
	float GetBorderSize() const { return _panel.GetBorderSize(); }
	const Colour& GetSelectionColour() const { return _selectionColour; }

	virtual void UpdateBounds() override;
	virtual void Render(RenderQueue&) const override;
	virtual void Update(float deltaTime) override;

	virtual bool OnKeyUp(bool blocked, EKeycode) override;
	virtual bool OnKeyDown(bool blocked, EKeycode) override;
	virtual bool OnCharInput(bool blocked, char) override;
	virtual bool OnMouseMove(bool blocked, float x, float y) override;
};
