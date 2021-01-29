#pragma once
#include <ELCore/String.hpp>
#include <ELCore/Types.hpp>
#include <WinSock2.h>

class Address
{
	sockaddr _sockaddr;

public:
	Address() : _sockaddr{} {}
	Address(const char* ipv4, ushort port);
	
	String GetIPV4() const;
	ushort GetPort() const;
};
