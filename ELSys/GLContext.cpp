#include "GLContext.hpp"
#include <ELCore/String.hpp>
#include "Debug.hpp"
#include "Window.hpp"

void GLContext::_Create(HDC hdc, int majorVersion, int minorVersion)
{
	int attribs[] = { 
		WGL_CONTEXT_MAJOR_VERSION_ARB, majorVersion, 
		WGL_CONTEXT_MINOR_VERSION_ARB, minorVersion, 
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB, 
		0
	};


	Delete();
	_id = wglCreateContextAttribsARB(hdc, 0, attribs);

	if (_id == NULL)
	{
		DWORD error = ::GetLastError();
		Debug::FatalError(CSTR("wglCreateContextAttribsARB failed (0x", String::FromInt(error, 2, 16), ")"));
	}
}

void GLContext::_Use(HDC hdc) const
{
	wglMakeCurrent(hdc, _id);
}

void GLContext::CreateDummyAndUse()
{
	Window::_EnsureWindowClasses();
	HWND hwnd = ::CreateWindow(Window::_wclassDummy, "temp", 0, 0, 0, 0, 0, NULL, NULL, Window::_programInstance, 0);
	HDC hdc = ::GetDC(hwnd);

	PIXELFORMATDESCRIPTOR pfd = { 0 };
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;
	pfd.cStencilBits = 8;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;

	int pfdId = ::ChoosePixelFormat(hdc, &pfd);

	::SetPixelFormat(hdc, pfdId, &pfd);

	_id = wglCreateContext(hdc);

	if (_id == NULL)
	{
		DWORD error = ::GetLastError();
		Debug::FatalError(CSTR("Dummy context creation: wglCreateContext failed (0x", String::FromInt(error, 2, 16), ")"));
	}

	_Use(hdc);

	::DestroyWindow(hwnd);
}

void GLContext::Delete()
{
	if (_id)
	{
		wglDeleteContext(_id);
		_id = 0;
	}
}
