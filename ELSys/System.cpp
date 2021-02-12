#include "System.hpp"
#include <Windows.h>
#include <iostream>

inline HCURSOR ECursorToHCURSOR(ECursor cursor)
{
	switch (cursor)
	{
	case ECursor::DEFAULT:
		return ::LoadCursor(NULL, IDC_ARROW);
	case ECursor::HAND:
		return ::LoadCursor(NULL, IDC_HAND);
	case ECursor::ARROWS_VERTICAL:
		return ::LoadCursor(NULL, IDC_SIZENS);
	case ECursor::ARROWS_HORIZONTAL:
		return ::LoadCursor(NULL, IDC_SIZEWE);
	case ECursor::ARROWS_QUAD:
		return ::LoadCursor(NULL, IDC_SIZEALL);
	case ECursor::IBEAM:
		return ::LoadCursor(NULL, IDC_IBEAM);
	}

	return NULL;
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
