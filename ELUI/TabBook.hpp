#pragma once
#include "Container.hpp"
#include "Element.hpp"
#include "Panel.hpp"
#include <ELCore/List.hpp>

class UITabBook;

class UITabPage : public UIContainer
{
protected:
	friend UITabBook;

	UIPanel _tab;
	
	UITabPage(UITabBook& parent);

public:
	virtual ~UITabPage() {}
};

class UITabBook : public UIElement
{
private:
	struct _UITabBookElement
	{
		UITabPage* page;
		UIPanel tab;

		String name;
	};

	List<_UITabBookElement> _tabs;

	virtual void _OnChildGained(UIElement* child) override;
	virtual void _OnChildLost(UIElement* child) override;

public:
	UITabBook(UIElement* parent = nullptr) : UIElement(parent) {}
	virtual ~UITabBook() {}

	virtual void Render(RenderQueue&) const override;

	UITabPage* GetTabWithName(const String& tabName);
};
