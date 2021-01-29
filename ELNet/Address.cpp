#include "Address.hpp"
#include <WS2tcpip.h>

Address::Address(const char* ipv4, ushort port) : _sockaddr{}
{
	sockaddr_in& address = (sockaddr_in&)_sockaddr;
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	inet_pton(AF_INET, ipv4, &address.sin_addr);
}

String Address::GetIPV4() const
{
	char buffer[64];

	sockaddr_in& inet = (sockaddr_in&)_sockaddr;
	if (inet_ntop(inet.sin_family, &inet.sin_addr, buffer, 64))
		return buffer;

	return "";
}

ushort Address::GetPort() const
{
	return ((sockaddr_in&)_sockaddr).sin_port;
}
