#pragma once
#include "EKeycode.hpp"
#include "GL.hpp"
#include <ELCore/MacroUtilities.hpp>
#include <ELCore/PagedMemory.hpp>
#include <ELCore/Types.hpp>
#include <ELMaths/Vector2.hpp>
#include <Windows.h>

namespace WindowFunctions_Win32
{
	inline void SetHWNDSizeAndPos(HWND hwnd, uint16 x, uint16 y, uint16 w, uint16 h) { ::SetWindowPos(hwnd, NULL, x, y, w, h, SWP_NOZORDER); }
	inline void ResizeHWND(HWND hwnd, uint16 w, uint16 h) { ::SetWindowPos(hwnd, NULL, 0, 0, w, h, SWP_NOREPOSITION | SWP_NOZORDER); }
	inline void RepositionHWND(HWND hwnd, uint16 x, uint16 y) { ::SetWindowPos(hwnd, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER); }

	void SetDefaultPixelFormat(HDC hdc);

	WPARAM SplitKeyWPARAMLeftRight(WPARAM wparam);

	LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	LRESULT CALLBACK WindowProc(class Window* window, HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	bool IsMouseInput(UINT msg);
	bool IsKeyInput(UINT msg);
}

struct WindowEvent
{
	enum Type
	{
		NONE = 0,
		CLOSED,
		RESIZE,
		FOCUS_GAINED,
		FOCUS_LOST,
		MOUSEMOVE,
		LEFTMOUSEDOWN,
		LEFTMOUSEUP,
		RIGHTMOUSEDOWN,
		RIGHTMOUSEUP,
		SCROLLWHEEL,
		KEYDOWN,
		KEYUP,
		CHAR,
		RAWINPUT
	} type;

	enum class RawInputType
	{
		MOUSE
	};

	union _extra
	{
		struct _resize
		{
			uint16 w;
			uint16 h;
		} resize;

		struct _mouseMove
		{
			uint16 x;
			uint16 y;
		} mouseMove;

		struct _scrollWheel
		{
			int16 lines;
		} scrollWheel;

		struct _keyDown
		{
			EKeycode key;
			bool isRepeat;
		} keyDown;

		struct _keyUp
		{
			EKeycode key;
		} keyUp;

		char character;

		struct _rawInput
		{
			RawInputType type;

			struct _mouse
			{
				int16 lastX;
				int16 lastY;
			} mouse;
		} rawInput;
	} data;

#pragma warning(suppress: 26812) //Yes, I know the enum is unscoped.
	WindowEvent(Type type = Type::NONE) : type(type), data() {}
};

enum class WindowFlags : uint16
{
	NONE = 0,
	OPENGL = 0x0001,
	BORDERLESS = 0x0002,
	FULLSCREEN = 0x0004
};

DEFINE_BITMASK_FUNCS(WindowFlags, uint16)

class Window
{
protected:
	HWND	_hwnd;		//Window handle
	HDC		_hdc;		//Device context handle

	bool _closeDestroysWindow;

	PagedMemory<> _eventMemory;
	List<WindowEvent, PagedMemory<>::Allocator<WindowEvent>> _eventList;

	static HINSTANCE _programInstance;
	static LPCTSTR _wclassDefault;
	static LPCTSTR _wclassDummy;

	static void _EnsureWindowClasses();

	friend class GLContext;
	friend void GL::LoadExtensions(const Window&);
	friend int WinMain(HINSTANCE, HINSTANCE, LPSTR, int);
	friend LRESULT CALLBACK WindowFunctions_Win32::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	friend LRESULT CALLBACK WindowFunctions_Win32::WindowProc(Window*, HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	//Yeah, that's right. This class is buddies with WinMain. :/
public:
	Window();
	Window(const Window&) = delete;
	Window(Window&&);

	virtual ~Window()
	{
		Destroy();
	}

	void Create(const char* title, const Window* parent = nullptr, WindowFlags flags = WindowFlags::NONE);
	void Show();
	void Hide();
	void SwapBuffers();
	void Destroy();

	void Focus();

	Vector2T<uint16> GetClientSize() const;
	
	void SetDestroyOnClose(bool destroyOnClose)							{ _closeDestroysWindow = destroyOnClose; }
	void SetTitle(const char *title)									{ ::SetWindowTextA(_hwnd, title); }
	void SetSizeAndPos(uint16 x, uint16 y, uint16 width, uint16 height)	{ WindowFunctions_Win32::SetHWNDSizeAndPos(_hwnd, x, y, width, height); }
	void SetSize(uint16 width, uint16 height)							{ WindowFunctions_Win32::ResizeHWND(_hwnd, width, height); }
	void SetPos(uint16 x, uint16 y)										{ WindowFunctions_Win32::RepositionHWND(_hwnd, x, y); }

	bool PollEvent(WindowEvent& out);

	bool IsVisible() const;

	static void Win32SetIconResource(int resource);
};
