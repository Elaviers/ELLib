#include "TabBook.hpp"

UITabPage::UITabPage(UITabBook& parent) : UIContainer(&parent) {}

void UITabBook::_OnChildGained(UIElement* child)
{
	UITabPage* page = dynamic_cast<UITabPage*>(child);
	if (page && !_tabs.Contains([page](const _UITabBookElement& a) { return a.page == page; }))
	{
		_UITabBookElement& tab = *_tabs.Emplace();
		tab.page = page;
	}
}

void UITabBook::_OnChildLost(UIElement* child)
{
	UITabPage* page = dynamic_cast<UITabPage*>(child);
	if (page)
		_tabs.Remove([page](const _UITabBookElement& element) { return element.page == page; }, false);
}

UITabPage* UITabBook::GetTabWithName(const String& tabName)
{
	for (const _UITabBookElement& element : _tabs)
		if (element.name == tabName)
			return element.page;

	return nullptr;
}

void UITabBook::Render(RenderQueue& renderQueue) const
{
	//todo

}
