#pragma once
#include <ELCore/Types.hpp>
#include <WinSock2.h>

class TCPSocket
{
private:
	SOCKET _socket;

public:
	TCPSocket() : _socket(INVALID_SOCKET) {}
	TCPSocket(const TCPSocket&) = delete;
	TCPSocket(TCPSocket&& other) noexcept : _socket(other._socket) { other._socket = INVALID_SOCKET; }

	bool IsOpen() const { return _socket != INVALID_SOCKET; }

	//Opens socket for listening on a port
	bool Listen(ushort port);

	//Attempts to accept connection while listening
	bool Accept(TCPSocket& socket, class Address& remote);
	
	bool ConnectTo(const char* ipv4, ushort port);
	void Close();

	bool Send(const byte* buffer, int size) const;

	//Returns bytes received
	size_t Receive(byte* buffer, int size) const;
};
