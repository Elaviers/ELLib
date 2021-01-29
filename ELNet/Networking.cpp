#include "Networking.hpp"
#include <ELCore/String.hpp>
#include <ELSys/Debug.hpp>
#include <WinSock2.h>

void Networking::Initialise()
{
	WSADATA wsaData;
	if (int wsaStatus = WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		Debug::Error(CSTR("WSAStartup failed with error code ", wsaStatus));
	}
}

void Networking::Cleanup()
{
	WSACleanup();
}
