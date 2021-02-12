#pragma once
#include "ECursor.hpp"

namespace System
{
	void SetCursor(ECursor cursor);

	//Opens console window and binds stdio/iostream to it
	bool OpenConsole();
	void CloseConsole();
}
