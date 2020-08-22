#pragma once
#include "Element.hpp"
#include <ELCore/Buffer.hpp>

class UIContainer : public UIElement
{
protected:
	friend UIElement;

	Buffer<UIElement*> _children;

	virtual void _OnBoundsChanged();

	virtual void _OnChildGained(UIElement *child) override;
	virtual void _OnChildLost(UIElement *child) override;

	void _SortChildren();

public:
	UIContainer(UIElement *parent = nullptr) : UIElement(parent) {}
	virtual ~UIContainer() {}

	const Buffer<UIElement*>& GetChildren() const { return _children; }

	void Clear();

	virtual void FocusElement(UIElement*) override;

	virtual void Render(RenderQueue&) const override;
	virtual void Update(float deltaTime) override;
	
	virtual bool OnKeyUp(EKeycode) override;
	virtual bool OnKeyDown(EKeycode) override;
	virtual bool OnCharInput(char) override;
	virtual bool OnMouseUp() override;
	virtual bool OnMouseDown() override;
	virtual bool OnMouseMove(float mouseX, float mouseY, bool blocked) override;
};
