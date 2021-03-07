#include "System.hpp"
#include <Windows.h>
#include <iostream>
#include <ELCore/MacroUtilities.hpp>

HCURSOR _Cursor_Arrow, _Cursor_Hand, _Cursor_ArrowsVert, _Cursor_ArrowsHoriz, _Cursor_ArrowsQuad, _Cursor_IBeam;

void _EnsureCursors()
{
	DO_ONCE_BEGIN;

	_Cursor_Arrow = ::LoadCursor(NULL, IDC_ARROW);
	_Cursor_Hand = ::LoadCursor(NULL, IDC_HAND);
	_Cursor_ArrowsVert = ::LoadCursor(NULL, IDC_SIZENS);
	_Cursor_ArrowsHoriz = ::LoadCursor(NULL, IDC_SIZEWE);
	_Cursor_ArrowsQuad = ::LoadCursor(NULL, IDC_SIZEALL);
	_Cursor_IBeam = ::LoadCursor(NULL, IDC_IBEAM);

	DO_ONCE_END;
}

ECursor HCURSORToECursor(HCURSOR cursor)
{
	_EnsureCursors();

	if (cursor == _Cursor_Arrow) return ECursor::DEFAULT;
	if (cursor == _Cursor_Hand) return ECursor::HAND;
	if (cursor == _Cursor_ArrowsVert) return ECursor::ARROWS_VERTICAL;
	if (cursor == _Cursor_ArrowsHoriz) return ECursor::ARROWS_HORIZONTAL;
	if (cursor == _Cursor_ArrowsQuad) return ECursor::ARROWS_QUAD;
	if (cursor == _Cursor_IBeam) return ECursor::IBEAM;

	return ECursor::NONE;
}

HCURSOR ECursorToHCURSOR(ECursor cursor)
{
	_EnsureCursors();

	switch (cursor)
	{
	case ECursor::DEFAULT: return _Cursor_Arrow;
	case ECursor::HAND: return _Cursor_Hand;
	case ECursor::ARROWS_VERTICAL: return _Cursor_ArrowsVert;
	case ECursor::ARROWS_HORIZONTAL: return _Cursor_ArrowsHoriz;
	case ECursor::ARROWS_QUAD: return _Cursor_ArrowsQuad;
	case ECursor::IBEAM: return _Cursor_IBeam;
	}

	return NULL;
}

ECursor System::GetCursor()
{
	return HCURSORToECursor(::GetCursor());
}

void System::SetCursor(ECursor cursor)
{
	HCURSOR hCursor = ECursorToHCURSOR(cursor);
	::SetCursor(hCursor);
}

bool System::OpenConsole()
{
	if (::AllocConsole())
	{
		//Reopen std streams
		FILE* fDummy;
		freopen_s(&fDummy, "CONOUT$", "w", stdout);
		freopen_s(&fDummy, "CONOUT$", "w", stderr);
		freopen_s(&fDummy, "CONIN$", "r", stdin);
		std::cout.clear(); std::clog.clear(); std::cerr.clear(); std::cin.clear();

		//Open wide char streams
		HANDLE hConOut = CreateFile(TEXT("CONOUT$"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		HANDLE hConIn = CreateFile(TEXT("CONIN$"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		::SetStdHandle(STD_OUTPUT_HANDLE, hConOut);
		::SetStdHandle(STD_ERROR_HANDLE, hConOut);
		::SetStdHandle(STD_INPUT_HANDLE, hConIn);
		std::wcout.clear(); std::wclog.clear(); std::wcerr.clear(); std::wcin.clear();
		return true;
	}

	return false;
}

void System::CloseConsole()
{
	::FreeConsole();
}
