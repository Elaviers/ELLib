#include "TCPSocket.hpp"
#include "UDPSocket.hpp"
#include <ELCore/String.hpp>
#include <ELSys/Debug.hpp>
#include <WS2tcpip.h>

PADDRINFOA ResolveAddress(const char* ipv4, ushort port)
{
	ADDRINFOA hints = {};
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_INET; //ipv4

	PADDRINFOA result;
	if (INT error = getaddrinfo(ipv4, String::FromInt(port).GetData(), &hints, &result))
	{
		Debug::Error(CSTR("Unable to resolve ipv4 address ", ipv4, ':', port, " (getaddrinfo returned ", error, ')'));
		return NULL;
	}

	return result;
}

bool SetSocketNonBlocking(SOCKET netsocket)
{
	u_long nonBlocking = 1;
	if (int error = ioctlsocket(netsocket, FIONBIO, &nonBlocking))
	{
		Debug::Error(CSTR("Unable to set net socket as non-blocking (", error, ')'));
		return false;
	}

	return true;
}

bool CloseSocket(SOCKET netsocket)
{
	if (netsocket != INVALID_SOCKET)
		if (int error = closesocket(netsocket))
		{
			Debug::Error(CSTR("Unable to close net socket (", error, ')'));
			return false;
		}
		else return true;

	return false;
}

bool TCPSocket::Listen(ushort port)
{
	PADDRINFOA addrinfo = ResolveAddress(nullptr, port);
	for (PADDRINFOA current = addrinfo; current; current = current->ai_next)
	{
		_socket = socket(current->ai_family, current->ai_socktype, current->ai_protocol);
		if (_socket == INVALID_SOCKET)
			continue;

		if (bind(_socket, current->ai_addr, (int)current->ai_addrlen))
		{
			closesocket(_socket);
			_socket = INVALID_SOCKET;
			continue;
		}
	}

	freeaddrinfo(addrinfo);

	if (_socket == INVALID_SOCKET)
	{
		Debug::Error(CSTR("Unable to create/bind a net socket for listening on port ", port, " (WSALastError=", WSAGetLastError(), ')'));
		return false;
	}

	if (!SetSocketNonBlocking(_socket))
	{
		Close();
		return false;
	}

	if (int error = listen(_socket, SOMAXCONN))
	{
		Debug::Error(CSTR("Unable to listen on a net socket (", error, ')'));
		Close();
		return false;
	}

	return true;
}

bool TCPSocket::Accept(TCPSocket& socket, Address& address)
{
	int size;
	SOCKET netsocket = accept(_socket, (sockaddr*)&address, &size);
	if (netsocket != INVALID_SOCKET)
	{
		socket._socket = netsocket;
		return true;
	}

	return false;
}

bool TCPSocket::ConnectTo(const char* ipv4, ushort port)
{
	PADDRINFOA addrinfo = ResolveAddress(nullptr, port);
	for (PADDRINFOA current = addrinfo; current; current = current->ai_next)
	{
		_socket = socket(current->ai_family, current->ai_socktype, current->ai_protocol);
		if (_socket == INVALID_SOCKET)
			continue;

		if (connect(_socket, current->ai_addr, (int)current->ai_addrlen))
		{
			closesocket(_socket);
			_socket = INVALID_SOCKET;
			continue;
		}
	}

	freeaddrinfo(addrinfo);

	if (_socket == INVALID_SOCKET)
	{
		Debug::Error(CSTR("Unable to create/connect a net socket to remote ", ipv4, ':', port, " (WSALastError=", WSAGetLastError(), ')'));
		return false;
	}

	if (!SetSocketNonBlocking(_socket))
	{
		Close();
		return false;
	}

	return true;
}

void TCPSocket::Close()
{
	if (CloseSocket(_socket))
		_socket = INVALID_SOCKET;
}

bool TCPSocket::Send(const byte* buffer, int size) const
{
	if (_socket != INVALID_SOCKET)
		if (send(_socket, (const char*)buffer, size, 0) == SOCKET_ERROR)
		{
			Debug::Error(CSTR("Network send failed (", WSAGetLastError(), ')'));
			return false;
		}
		else return true;

	return false;
}

size_t TCPSocket::Receive(byte* buffer, int size) const
{
	return _socket != INVALID_SOCKET ? recv(_socket, (char*)buffer, size, 0) : 0;
}






bool UDPSocket::Open(ushort port)
{
	PADDRINFOA addrinfo = ResolveAddress(nullptr, port);
	for (PADDRINFOA current = addrinfo; current; current = current->ai_next)
	{
		_socket = socket(current->ai_family, current->ai_socktype, current->ai_protocol);
		if (_socket == INVALID_SOCKET)
			continue;

		if (bind(_socket, current->ai_addr, (int)current->ai_addrlen))
		{
			closesocket(_socket);
			_socket = INVALID_SOCKET;
			continue;
		}
	}

	freeaddrinfo(addrinfo);

	if (_socket == INVALID_SOCKET)
	{
		Debug::Error(CSTR("Unable to create/bind a net socket on port ", port, " (WSALastError=", WSAGetLastError(), ')'));
		return false;
	}

	if (!SetSocketNonBlocking(_socket))
	{
		Close();
		return false;
	}

	return true;
}

void UDPSocket::Close()
{
	if (CloseSocket(_socket))
		_socket = INVALID_SOCKET;
}

bool UDPSocket::SendTo(const Address& to, const byte* buffer, int size) const
{
	if (_socket != INVALID_SOCKET)
	{
		if (sendto(_socket, (const char*)buffer, size, 0, (const sockaddr*)&to, sizeof(sockaddr)) == SOCKET_ERROR)
		{
			return false;
		}
	}

	return true;
}

size_t UDPSocket::Receive(Address& from, byte* buffer, int size) const
{
	if (_socket != INVALID_SOCKET)
	{
		int length = 0;
		int amount = recvfrom(_socket, (char*)buffer, size, 0, (sockaddr*)&from, &length);

		if (amount == SOCKET_ERROR)
			return 0;

		return amount;
	}

	return 0;
}
