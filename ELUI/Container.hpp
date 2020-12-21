#pragma once
#include "Element.hpp"
#include <ELCore/Buffer.hpp>

class UIContainer : public UIElement
{
protected:
	friend UIElement;

	Buffer<UIElement*> _children;

	virtual void _OnChildGained(UIElement *child) override;
	virtual void _OnChildLost(UIElement *child) override;

	void _SortChildren();

public:
	UIContainer(UIElement *parent = nullptr) : UIElement(parent) {}
	virtual ~UIContainer() {}

	const Buffer<UIElement*>& GetChildren() const { return _children; }

	void Clear();

	virtual void FocusElement(UIElement*) override;

	virtual void UpdateBounds() override;
	virtual void Render(RenderQueue&) const override;
	virtual void Update(float deltaTime) override;
	
	virtual bool OnKeyUp(bool blocked, EKeycode) override;
	virtual bool OnKeyDown(bool blocked, EKeycode) override;
	virtual bool OnCharInput(bool blocked, char) override;
	virtual bool OnMouseMove(bool blocked, float x, float y) override;
};
