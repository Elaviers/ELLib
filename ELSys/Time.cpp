#include "Time.hpp"
#include <Windows.h>

uint32 Time::GetRandSeed()
{
	SYSTEMTIME time;
	::GetSystemTime(&time);

	return (uint32)(time.wMilliseconds + 4223) * (time.wSecond + 1) * (time.wHour + 1);
}

void Time::Sleep(int millis)
{
	::Sleep((DWORD)millis);
}
