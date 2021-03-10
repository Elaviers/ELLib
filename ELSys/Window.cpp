#include "Window.hpp"
#include "Debug.hpp"
#include <ELCore/String.hpp>
#include <windowsx.h>

void WindowFunctions::SetDefaultPixelFormat(HDC hdc)
{
	const int attribInts[] =
	{
		WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
		WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
		WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
		WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
		WGL_COLOR_BITS_ARB, 32,
		WGL_DEPTH_BITS_ARB, 24,
		WGL_STENCIL_BITS_ARB, 8,
		WGL_SAMPLE_BUFFERS_ARB, 1,
		WGL_SAMPLES_ARB, 8,					//8x MSAA
		0, // End
	};

	int pfdId;
	UINT formatCount;

	wglChoosePixelFormatARB(hdc, attribInts, NULL, 1, &pfdId, &formatCount);

	PIXELFORMATDESCRIPTOR pfd = {0};
	DescribePixelFormat(hdc, pfdId, sizeof(pfd), &pfd);
	Debug::Assert(SetPixelFormat(hdc, pfdId, &pfd), CSTR("SetPixelFormat returned FALSE (0x", String::FromInt(::GetLastError(), 2, 16), ")"));
}

WPARAM WindowFunctions::SplitKeyWPARAMLeftRight(WPARAM wparam)
{
	switch (wparam)
	{
	case VK_CONTROL:
		return ::GetKeyState(VK_RCONTROL) & 0x8000 ? VK_RCONTROL : VK_LCONTROL;
	case VK_SHIFT:
		return ::GetKeyState(VK_RSHIFT) & 0x8000 ? VK_RSHIFT : VK_LSHIFT;
	case VK_MENU:
		return ::GetKeyState(VK_RMENU) & 0x8000 ? VK_RMENU : VK_LMENU;
	}

	return wparam;
}

HINSTANCE Window::_programInstance = NULL;
LPCTSTR Window::_wclassDefault = TEXT("DEFAULTENGINEWINDOW");
LPCTSTR Window::_wclassDummy = TEXT("DUMMY");

LPCSTR iconResource = 0;

void Window::_EnsureWindowClasses()
{
	DO_ONCE_BEGIN;
	HICON icon = iconResource ? ::LoadIcon(_programInstance, iconResource) : ::LoadIcon(NULL, IDI_APPLICATION);

	WNDCLASSEX windowClass = {};
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.lpfnWndProc = ::DefWindowProc;
	windowClass.hInstance = _programInstance;
	windowClass.lpszClassName = _wclassDummy;
	Debug::Assert(::RegisterClassEx(&windowClass), CSTR("Unable to register dummy window class (0x", String::FromInt(::GetLastError(), 2, 16), ")"));

	windowClass.lpfnWndProc = _WindowsProc;
	windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	windowClass.hIcon = icon;
	windowClass.hIconSm = icon;
	windowClass.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	windowClass.lpszClassName = _wclassDefault;
	Debug::Assert(::RegisterClassEx(&windowClass), CSTR("Unable to register default window class (0x", String::FromInt(::GetLastError(), 2, 16), ")"));
	DO_ONCE_END;
}

void Window::Win32SetIconResource(int resource)
{
	iconResource = MAKEINTRESOURCE(resource);
}

Window::Window() : 
	_hwnd(NULL), _hdc(NULL), 
	_closeDestroysWindow(true),
	_eventList(NewHandler(_eventPool, &_EventPoolType::NewArray), DeleteHandler(_eventPool, &_EventPoolType::DeleteHandler))
{
}

void Window::Create(const char* title, const Window* parent, WindowFlags flags)
{
	_EnsureWindowClasses();

	if (_hwnd)
		::DestroyWindow(_hwnd);

	DWORD style = 0;

	bool borderless = (int)(flags & WindowFlags::BORDERLESS);
	if (borderless)
		style |= WS_POPUP;
	else
		style |= WS_OVERLAPPEDWINDOW;

	_hwnd = ::CreateWindow(_wclassDefault, title, style, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, parent ? parent->_hwnd : NULL, NULL, _programInstance, (LPVOID)this);

	Debug::Assert(_hwnd, CSTR("Unable to create window (0x", String::FromInt(::GetLastError(), 2, 16), ")"));

	if ((int)(flags & WindowFlags::FULLSCREEN))
	{
		if (borderless)
		{
			MONITORINFO monitorInfo;
			monitorInfo.cbSize = sizeof(monitorInfo);
			GetMonitorInfo(MonitorFromWindow(_hwnd, MONITOR_DEFAULTTONEAREST), &monitorInfo);

			WindowFunctions::SetHWNDSizeAndPos(_hwnd, 0, 0, monitorInfo.rcMonitor.right, monitorInfo.rcMonitor.bottom);
		}
		else
		{
			//todo.. heh
		}
	}

	_hdc = GetDC(_hwnd);
	WindowFunctions::SetDefaultPixelFormat(_hdc);
}

void Window::Show()
{
	if (_hwnd)
		::ShowWindow(_hwnd, SW_SHOW);
}

void Window::Hide()
{
	if (_hwnd)
		::ShowWindow(_hwnd, SW_HIDE);
}

void Window::Destroy()
{
	if (_hwnd)
	{
		::DestroyWindow(_hwnd);
		_hwnd = NULL;
	}
}

void Window::SwapBuffers()
{
	::SwapBuffers(_hdc);
}

void Window::Focus()
{
	::SetFocus(_hwnd);
}

Vector2T<uint16> Window::GetClientSize() const
{
	RECT sz = {};
	::GetClientRect(_hwnd, &sz);

	return Vector2T<uint16>(sz.right, sz.bottom);
}

bool Window::PollEvent(WindowEvent& out)
{
	MSG msg;
	BOOL result = ::PeekMessage(&msg, _hwnd, 0, 0, PM_REMOVE);
	if (result)
	{
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}

	if (_eventList.begin().IsValid())
	{
		out = *_eventList.begin();
		if (!_eventList.RemoveIndex(0).IsValid())
			_eventPool.Clear();

		return true;
	}

	return false;
}

#include <ELCore/Buffer.hpp>

LRESULT CALLBACK Window::_WindowsProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (msg == WM_CREATE)
	{
		LPCREATESTRUCT create = (LPCREATESTRUCT)lparam;
		::SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)create->lpCreateParams);
		return 0;
	}

	Window* window = (Window*)::GetWindowLongPtr(hwnd, GWLP_USERDATA);
	if (window == nullptr)
		return ::DefWindowProc(hwnd, msg, wparam, lparam);

	switch (msg)
	{
	case WM_CLOSE:
		if (window->_closeDestroysWindow)
			::DestroyWindow(hwnd);
		else
			window->_eventList.Emplace(WindowEvent::CLOSED);

		break;

	case WM_DESTROY:
		if (window->_closeDestroysWindow)
			window->_eventList.Emplace(WindowEvent::CLOSED);
		
		break;

	case WM_SIZE:
	{
		bool done = false;
		for (WindowEvent& existing : window->_eventList)
			if (existing.type == WindowEvent::RESIZE)
			{
				existing.data.resize.w = LOWORD(lparam);
				existing.data.resize.h = HIWORD(lparam);
				done = true;
				break;
			}

		if (!done)
		{
			auto& e = *window->_eventList.Emplace(WindowEvent::RESIZE);
			e.data.resize.w = LOWORD(lparam);
			e.data.resize.h = HIWORD(lparam);
		}
	}
		break;
		
	case WM_SETFOCUS:
		window->_eventList.Emplace(WindowEvent::FOCUS_GAINED);
		break;

	case WM_KILLFOCUS:
		window->_eventList.Emplace(WindowEvent::FOCUS_LOST);
		break;

	case WM_MOUSEMOVE:
	{
		bool done = false;
		for (WindowEvent& existing : window->_eventList)
			if (existing.type == WindowEvent::MOUSEMOVE)
			{
				existing.data.mouseMove.x = GET_X_LPARAM(lparam);
				existing.data.mouseMove.y = GET_Y_LPARAM(lparam);
				done = true;
				break;
			}

		if (!done)
		{
			auto& e = *window->_eventList.Emplace(WindowEvent::MOUSEMOVE);
			e.data.mouseMove.x = GET_X_LPARAM(lparam);
			e.data.mouseMove.y = GET_Y_LPARAM(lparam);
		}
	}
		break;

	case WM_LBUTTONDOWN:
		window->_eventList.Emplace(WindowEvent::LEFTMOUSEDOWN);
		break;

	case WM_LBUTTONUP:
		window->_eventList.Emplace(WindowEvent::LEFTMOUSEUP);
		break;

	case WM_RBUTTONDOWN:
		window->_eventList.Emplace(WindowEvent::RIGHTMOUSEDOWN);
		break;

	case WM_RBUTTONUP:
		window->_eventList.Emplace(WindowEvent::RIGHTMOUSEUP);
		break;

	case WM_MOUSEWHEEL:
	{
		auto& e = *window->_eventList.Emplace(WindowEvent::SCROLLWHEEL);
		e.data.scrollWheel.lines = (int16)HIWORD(wparam) / WHEEL_DELTA;
	}
		break;

	case WM_KEYDOWN:
	{
		auto& e = *window->_eventList.Emplace(WindowEvent::KEYDOWN);
		e.data.keyDown.key = (EKeycode)WindowFunctions::SplitKeyWPARAMLeftRight(wparam);
		e.data.keyDown.isRepeat = lparam & (1 << 30);
	}
		break;

	case WM_KEYUP:
	{
		auto& e = *window->_eventList.Emplace(WindowEvent::KEYUP);
		e.data.keyUp.key = (EKeycode)WindowFunctions::SplitKeyWPARAMLeftRight(wparam);
	}
		break;

	case WM_CHAR:
	{
		auto& e = *window->_eventList.Emplace(WindowEvent::CHAR);
		e.data.character = (char)wparam;
	}
		break;

	case WM_INPUT:
	{
		static Buffer<byte> buffer;

		UINT size = 0;
		GetRawInputData((HRAWINPUT)lparam, RID_INPUT, NULL, &size, sizeof(RAWINPUTHEADER));

		if (buffer.GetSize() < size)
			buffer.SetSize(size);

		GetRawInputData((HRAWINPUT)lparam, RID_INPUT, &buffer[0], &size, sizeof(RAWINPUTHEADER));

		RAWINPUT* input = (RAWINPUT*)buffer.Elements();
		if (input->header.dwType == RIM_TYPEMOUSE)
		{
			bool done = false;
			for (WindowEvent& existing : window->_eventList)
				if (existing.type == WindowEvent::RAWINPUT && existing.data.rawInput.type == WindowEvent::RawInputType::MOUSE)
				{
					existing.data.rawInput.mouse.lastX += (int16)input->data.mouse.lLastX;
					existing.data.rawInput.mouse.lastY += (int16)input->data.mouse.lLastY;
					done = true;
					break;
				}

			if (!done)
			{
				auto& e = *window->_eventList.Emplace(WindowEvent::RAWINPUT);
				e.data.rawInput.type = WindowEvent::RawInputType::MOUSE;
				e.data.rawInput.mouse.lastX = (int16)input->data.mouse.lLastX;
				e.data.rawInput.mouse.lastY = (int16)input->data.mouse.lLastY;
			}
		}
	}
	break;

	default: return ::DefWindowProc(hwnd, msg, wparam, lparam);
	}

	return 0;
}

