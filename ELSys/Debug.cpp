#include "Debug.hpp"
#include <ELCore/String.hpp>
#include <iostream>
#include <Windows.h>

namespace Debug
{
	void Break()
	{
		::DebugBreak();
	}

	void Print(const char* string, const char* tag)
	{
		::OutputDebugStringA(CSTR(tag, string));

		if (tag)
			std::cout << tag << string;
		else
			std::cout << string;
	}

	void PrintLine(const char* string, const char* tag)
	{
		::OutputDebugStringA(CSTR(tag, string, '\n'));

		if (tag)
			std::cout << tag << string << '\n';
		else
			std::cout << string << '\n';
	}

	void Error(const char* string)
	{
		PrintLine(string, "[ERROR] ");
		Message(string, "Error");
	}

	void FatalError(const char* string)
	{
		PrintLine(string, "[FATAL] ");
		Message(string, "Fatal Error");

		Break();

		::ExitProcess(1);
	}

	void Message(const char* message, const char* title)
	{
		::MessageBoxA(NULL, message, title, MB_OK);
		PrintLine(message, "[MESSAGE] ");
	}
}
