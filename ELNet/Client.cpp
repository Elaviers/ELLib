#include "Client.hpp"
#include "Server.hpp"

Client Client::MakeServerConnection(const Address& server)
{
	String serverIP = server.GetIPV4();
	ushort serverPort = server.GetPort();

	Client client(false);
	client._localSlot = 0;
	client._remote = server;
	client._tcp.ConnectTo(serverIP.GetData(), serverPort);
	
	//todo: hard-coded client UDP port
	client._udp.Open(7777);

	return client;
}

Client Client::MakeLocalServerConnection(Server& server)
{
	Client client(true);
	client._localSlot = server.OpenSlotLocally();

	return client;
}

bool Client::Send(const Buffer<byte>& message, ENetProtocol protocol) const
{
	switch (protocol)
	{
	case ENetProtocol::TCP:
		return _tcp.Send(message.Elements(), (int)message.GetSize());
	case ENetProtocol::UDP:
		return _udp.SendTo(_remote, message.Elements(), (int)message.GetSize());
	}

	return false;
}

bool Client::Receive(Buffer<byte>& message)
{
	if (_tcpDigest.ReadNext(_tcp))
	{
		message = _tcpDigest.RetrieveMessage();
		return true;
	}

	return false;
}
