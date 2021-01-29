#pragma once
#include "Address.hpp"
#include "ENetProtocol.hpp"
#include "TCPSocket.hpp"
#include "TCPDigest.hpp"
#include "UDPSocket.hpp"

class Server;

class Client
{
private:

	//-1 = not connected
	int32 _localSlot;
	bool _isLocalClient;

	Address _remote;
	UDPSocket _udp;

	TCPSocket _tcp;
	TCPDigest _tcpDigest;

protected:
	Client(bool isLocalClient) : _localSlot(-1), _isLocalClient(isLocalClient) {}

public:
	static Client MakeServerConnection(const Address& server);
	static Client MakeLocalServerConnection(Server& server);

	bool Send(const Buffer<byte>& message, ENetProtocol protocol) const;
	bool Receive(Buffer<byte>& message);
};
