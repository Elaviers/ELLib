#pragma once
#include "GL.hpp"
#include "Window.hpp"

class GLContext
{
private:
	HGLRC _id;

	void _Create(HDC, int majorVersion, int minorVersion);
	void _Use(HDC) const;

public:
	void Use_Win32(HDC hdc) const { _Use(hdc); }

public:
	GLContext() : _id(0) {}
	GLContext(const GLContext&) = delete;
	GLContext(GLContext&& other) noexcept : _id(other._id) { other._id = 0; }
	~GLContext() { Delete(); }

	void CreateDummyAndUse();
	void Delete();
	
	void Create(const Window &window, int majorVersion = 4, int minorVersion = 1) { _Create(window._hdc, majorVersion, minorVersion); }
	void Use(const Window &window) const { _Use(window._hdc); }

	bool IsValid() { return _id != 0; }
};
