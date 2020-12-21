#pragma once
#include <ELCore/Event.hpp>
#include <ELSys/ECursor.hpp>
#include <ELSys/EKeycode.hpp>

class RenderQueue;

struct UICoord
{
	float relative;
	float absolute;

	UICoord(float relative = 0.f) : relative(relative), absolute(0.f) {}
	UICoord(float relative, float absolute) : relative(relative), absolute(absolute) {}
};

struct UIBounds
{
	UICoord x;
	UICoord y;
	UICoord w;
	UICoord h;

	UIBounds() : x(0.f), y(0.f), w(1.f), h(1.f) {}
	UIBounds(const UICoord& x, const UICoord& y, const UICoord& w, const UICoord& h) : x(x), y(y), w(w), h(h) {}
};

struct AbsoluteBounds
{
	float x, y, w, h;

	AbsoluteBounds() : x(0.f), y(0.f), w(0.f), h(0.f) {}
};

class UIElement
{
	void _SetFocus(UIElement& element) { _hasFocus = true; }
	void _ClearFocus(UIElement& element) { _hasFocus = false; }

protected:
	UIElement* _parent;

	bool _markedForDelete;
	bool _hasFocus;
	bool _hover;

	bool _focusOnClick;

	UIBounds _bounds;
	AbsoluteBounds _absoluteBounds;
	AbsoluteBounds _clientBounds;

	float _z;

	ECursor _cursor;

public:
	Event<UIElement&> onFocusGained;
	Event<UIElement&> onFocusLost;

protected:
	UIElement(UIElement* parent);
	UIElement(const UIElement& other) = delete;

	virtual void _UpdateClientBounds();
	
	virtual void _OnChildGained(UIElement *child) {}
	virtual void _OnChildLost(UIElement *child) {}

public:
	virtual ~UIElement();


	UIElement* GetParent() const { return _parent; }
	const AbsoluteBounds& GetAbsoluteBounds() const { return _absoluteBounds; }
	const AbsoluteBounds& GetClientBounds() const { return _clientBounds; }
	const UIBounds& GetBounds() const { return _bounds; }
	float GetZ() const { return _z; }
	const ECursor& GetCursor() const { return _cursor; }
	bool IsMarkedForDelete() const { return _markedForDelete; }
	bool HasFocus() const { return _hasFocus; }
	bool GetFocusOnClick() const { return _focusOnClick; }

	UIElement& SetParent(UIElement* parent);
	UIElement& SetBounds(const UIBounds& bounds) { _bounds = bounds; UpdateBounds(); return *this; }
	UIElement& SetX(const UICoord& x) { _bounds.x = x; UpdateBounds(); return *this; }
	UIElement& SetY(const UICoord& y) { _bounds.y = y; UpdateBounds(); return *this; }
	UIElement& SetW(const UICoord& w) { _bounds.w = w; UpdateBounds(); return *this; }
	UIElement& SetH(const UICoord& h) { _bounds.h = h; UpdateBounds(); return *this; }
	UIElement& SetZ(float z);
	UIElement& SetCursor(const ECursor& cursor) { _cursor = cursor; return *this; }
	UIElement& SetFocusOnClick(bool focusOnClick) { _focusOnClick = focusOnClick; return *this; }
	void MarkForDelete() { _markedForDelete = true; }

	//Fires onmousemove with coordinates that cannot possibly overlap anything
	void ResetMouseMove() { OnMouseMove(false, INFINITY, INFINITY); }

	//True if mouse is currently hovering over the element
	bool GetHover() const { return _hover; }

	void RequestFocus();

	virtual bool OverlapsPoint(float x, float y) const
	{
		return x >= _absoluteBounds.x &&
			x <= (_absoluteBounds.x + _absoluteBounds.w) &&
			y >= _absoluteBounds.y &&
			y <= (_absoluteBounds.y + _absoluteBounds.h);
	}

	virtual void FocusElement(UIElement* child);

	virtual void UpdateBounds();
	virtual void Render(RenderQueue&) const {}
	virtual void Update(float deltaTime) {}

	virtual void OnHoverStart() {}
	virtual void OnHoverStop() {}

	//These functions return true if they 'consume' the input
	virtual bool OnKeyUp(bool blocked, EKeycode);
	virtual bool OnKeyDown(bool blocked, EKeycode);
	virtual bool OnCharInput(bool blocked, char) { return false; }
	virtual bool OnMouseMove(bool blocked, float x, float y);

	bool KeyDown(EKeycode key) { return OnKeyDown(false, key); }
	bool KeyUp(EKeycode key) { return OnKeyUp(false, key); }
	bool InputChar(char c) { return OnCharInput(false, c); }
	bool SetCursorPos(float x, float y) { return OnMouseMove(false, x, y); }
};
