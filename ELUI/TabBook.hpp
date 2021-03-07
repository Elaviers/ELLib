#pragma once
#include "Container.hpp"
#include "Element.hpp"
#include "Label.hpp"
#include "Panel.hpp"
#include <ELCore/List.hpp>

class UITabBook : public UIElement
{
public:
	enum class TabBarPosition
	{
		NOWHERE = 0,
		TOP,
		BOTTOM,
		LEFT,
		RIGHT
	};

private:
	struct _UITabBookElement
	{
		UIElement* page;
		UIPanel panel;
		UILabel label;

		_UITabBookElement() : page(nullptr) {}
	};

	bool _createTabsFromChildren;

	TabBarPosition _tabBarPosition;
	float _tabBarSize;
	float _minimumTabSize;
	float _maximumTabSize;

	List<_UITabBookElement> _tabs;
	_UITabBookElement* _hoverTab;
	_UITabBookElement* _activeTab;

	float _tabBorderSize;
	SharedPointer<const Material> _tabMaterial;
	SharedPointer<const Material> _tabMaterialHover;
	SharedPointer<const Material> _tabMaterialActive;
	
	UIColour _tabColour;
	UIColour _tabColourHover;
	UIColour _tabColourActive;
	
	SharedPointer<const Font> _tabFont;
	UIColour _tabFontColour;
	UIColour _tabFontColourHover;
	UIColour _tabFontColourActive;

	virtual void _UpdateClientBounds() override;

	virtual void _OnChildGained(UIElement* child) override;
	virtual void _OnChildLost(UIElement* child) override;

	void _UpdateTabs();

public:
	UITabBook(UIElement* parent = nullptr) : 
		UIElement(parent), 
		_createTabsFromChildren(true),
		_tabBarPosition(TabBarPosition::TOP), _tabBarSize(32.f),
		_minimumTabSize(256.f), _maximumTabSize(512.f),
		_hoverTab(nullptr), _activeTab(nullptr),
		_tabBorderSize(8.f), 
		_tabColour(Colour::White), _tabColourHover(Colour::White), _tabColourActive(Colour::White),
		_tabFontColour(Colour::White), _tabFontColourHover(Colour::White), _tabFontColourActive(Colour::White)
	{}

	virtual ~UITabBook() {}

	UIElement* GetTabWithName(const Text& name);
	Text GetTabName(const UIElement& tab) const;

	bool SetActiveTab(const UIElement* tab);
	bool SetTabName(const UIElement& tab, const Text& name);

	TabBarPosition GetTabBarPosition() const { return _tabBarPosition; }
	float GetTabBarSize() const { return _tabBarSize; }
	float GetMinimumTabSize() const { return _minimumTabSize; }
	float GetMaximumTabSize() const { return _maximumTabSize; }
	float GetTabBorderSize() const { return _tabBorderSize; }
	const UIColour& GetTabColour() const { return _tabColour; }
	const UIColour& GetTabColourHover() const { return _tabColourHover; }
	const UIColour& GetTabColourSelected() const { return _tabColourActive; }
	const SharedPointer<const Material>& GetTabMaterial() const { return _tabMaterial; }
	const SharedPointer<const Material>& GetTabMaterialHover() const { return _tabMaterialHover; }
	const SharedPointer<const Material>& GetTabMaterialSelected() const { return _tabMaterialActive; }
	const SharedPointer<const Font>& GetTabFont() const { return _tabFont; }
	const UIColour& GetTabFontColour() const { return _tabFontColour; }
	const UIColour& GetTabFontColourHover() const { return _tabFontColourHover; }
	const UIColour& GetTabFontColourSelected() const { return _tabFontColourActive; }

	UITabBook& SetTabBarPosition(TabBarPosition position) { _tabBarPosition = position; _UpdateTabs(); _UpdateClientBounds(); return *this; }
	UITabBook& SetTabBarSize(float tabBarSize) { _tabBarSize = tabBarSize; _UpdateTabs(); _UpdateClientBounds(); return *this; }
	UITabBook& SetMinimumTabSize(float minimumTabSize) { _minimumTabSize = minimumTabSize; _UpdateTabs(); return *this; }
	UITabBook& SetMaximumTabSize(float maximumTabSize) { _maximumTabSize = maximumTabSize; _UpdateTabs(); return *this; }
	UITabBook& SetTabBorderSize(float size) { _tabBorderSize = size; _UpdateTabs(); return *this; }
	UITabBook& SetTabColour(const UIColour& colour) { _tabColour = colour; _UpdateTabs(); return *this; }
	UITabBook& SetTabColourHover(const UIColour& colour) { _tabColourHover = colour; _UpdateTabs(); return *this; }
	UITabBook& SetTabColourSelected(const UIColour& colour) { _tabColourActive = colour; _UpdateTabs(); return *this; }
	UITabBook& SetTabMaterial(const SharedPointer<const Material>& material) { _tabMaterial = material; _UpdateTabs(); return *this; }
	UITabBook& SetTabMaterialHover(const SharedPointer<const Material>& material) { _tabMaterialHover = material; _UpdateTabs(); return *this; }
	UITabBook& SetTabMaterialSelected(const SharedPointer<const Material>& material) { _tabMaterialActive = material; _UpdateTabs(); return *this; }
	UITabBook& SetTabFont(const SharedPointer<const Font>& font) { _tabFont = font; _UpdateTabs(); return *this; }
	UITabBook& SetTabFontColour(const UIColour& colour) { _tabFontColour = colour; _UpdateTabs(); return *this; }
	UITabBook& SetTabFontColourHover(const UIColour& colour) { _tabFontColourHover = colour; _UpdateTabs(); return *this; }
	UITabBook& SetTabFontColourSelected(const UIColour& colour) { _tabFontColourActive = colour; _UpdateTabs(); return *this; }

	virtual void FocusElement(UIElement*) override;

	virtual void UpdateBounds() override;
	virtual void Render(RenderQueue&) const override;

	virtual void Update(float deltaTime) override;

	virtual bool OnKeyUp(bool blocked, EKeycode) override;
	virtual bool OnKeyDown(bool blocked, EKeycode) override;
	virtual bool OnCharInput(bool blocked, char) override;
	virtual bool OnMouseMove(bool blocked, float x, float y) override;
};
