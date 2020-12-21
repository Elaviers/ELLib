#pragma once
#include "Element.hpp"
#include "Panel.hpp"
#include "Label.hpp"
#include "Rect.hpp"
#include <ELCore/List.hpp>

class UIListBox : public UIElement
{
	UIPanel _panel;
	List<UILabel> _labels;

	UILabel* _selectedLabel;
	UIRect _selectionBox;

	SharedPointer<const Font> _font;
	UIColour _textColour;
	UIColour _textShadowColour;
	Vector2 _textShadowOffset;
	ETextAlignment _textAlignment;
	float _textMargin;
	float _itemHeight;

	void _UpdateLabels();

public:
	Event<UILabel&> onSelectionChanged;
	Event<UILabel&> onSelectionSubmitted;

	UIListBox(UIElement* parent = nullptr) : UIElement(parent), _panel(this), _selectionBox(this), _selectedLabel(nullptr), _itemHeight(32.f) 
	{
		SetSelectionColour(Colour(0.f, .2f, 1.f, .2f));
	}
	virtual ~UIListBox() {}

	const List<UILabel>& GetItems() const { return _labels; }

	void Add(const Text& item);

	float GetBorderSize() const { return _panel.GetBorderSize(); }
	const SharedPointer<const Material>& GetMaterial() const { return _panel.GetMaterial(); }
	const UIColour& GetColour() const { return _panel.GetColour(); }
	float GetItemHeight() const { return _itemHeight; }
	const SharedPointer<const Font>& GetFont() const { return _font; }
	const UIColour& GetTextColour() const { return _textColour; }
	const UIColour& GetTextShadowColour() const { return _textShadowColour; }
	const Vector2& GetTextShadowOffset() const { return _textShadowOffset; }
	ETextAlignment GetTextAlignment() const { return _textAlignment; }
	float GetTextMargin() const { return _textMargin; }
	const UILabel* GetSelectedLabel() const { return _selectedLabel; }
	const UIColour& GetSelectionColour() const { return _selectionBox.GetColour(); }

	UIListBox& SetBorderSize(float borderSize) { _panel.SetBorderSize(borderSize); return *this; }
	UIListBox& SetMaterial(const SharedPointer<const Material>& material) { _panel.SetMaterial(material); return *this; }
	UIListBox& SetColour(const UIColour& colour) { _panel.SetColour(colour); return *this; }
	UIListBox& SetItemHeight(float itemHeight) { _itemHeight = itemHeight; _UpdateLabels(); return *this; }
	UIListBox& SetFont(const SharedPointer<const Font>& font) { _font = font; _UpdateLabels(); return *this; }
	UIListBox& SetTextColour(const UIColour& colour) { _textColour = colour; _UpdateLabels(); return *this; }
	UIListBox& SetTextShadowColour(const UIColour& colour) { _textShadowColour = colour; _UpdateLabels(); return *this; }
	UIListBox& SetTextShadowOffset(const Vector2& offset) { _textShadowOffset = offset; _UpdateLabels(); return *this; }
	UIListBox& SetTextAlignment(ETextAlignment alignment) { _textAlignment = alignment; _UpdateLabels(); return *this; }
	UIListBox& SetTextMargin(float margin) { _textMargin = margin; _UpdateLabels(); return *this; }
	UIListBox& SetSelectionColour(const UIColour& colour) { _selectionBox.SetColour(colour); return *this; }

	virtual void UpdateBounds() override
	{
		UIElement::UpdateBounds();
		_panel.UpdateBounds();
		for (UILabel& label : _labels)
			label.UpdateBounds();
	}

	virtual void Render(RenderQueue& q) const override
	{
		_panel.Render(q);
		for (const UILabel& label : _labels)
			label.Render(q);

		if (_selectedLabel)
			_selectionBox.Render(q);
	}

	virtual bool OnKeyUp(bool blocked, EKeycode) override;
	virtual bool OnKeyDown(bool blocked, EKeycode) override;
	virtual bool OnMouseMove(bool blocked, float x, float y) override;
};
