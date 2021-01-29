#include "Server.hpp"
#include <ELSys/Debug.hpp>

void Server::ServerClient::Close()
{
	isLocalClient = false;
	tcp.Close();
	address = Address();
}

void Server::_UpdateNextClientSlot()
{
	for (_nextClientSlot = 0; _nextClientSlot < _clients.GetSize(); ++_nextClientSlot)
		if (!_clients[_nextClientSlot].isLocalClient && !_clients[_nextClientSlot].tcp.IsOpen())
			return;

	_nextClientSlot = INVALID_SLOT;
}

void Server::_OnClientConnected(uint32 slot)
{
	_clients[slot].tcpDigest.Reset();
}

bool Server::Open(ushort listenPort)
{
	bool btcp = _tcp.Listen(listenPort);
	bool budp = _udp.Open(listenPort);

	return btcp && budp;
}

void Server::Close()
{
	for (ServerClient& client : _clients)
		client.Close();

	_tcp.Close();
	_udp.Close();
}

void Server::SetSize(uint32 size)
{
	if (size > 0)
	{



	}
	else Debug::Error(CSTR("Server::SetSize called with size of ", size, "! Why?!?!?"));
}

uint32 Server::OpenSlotLocally()
{
	uint32 slot = _nextClientSlot;
	_clients[slot].isLocalClient = true;
	_UpdateNextClientSlot();
	_OnClientConnected(slot);
	return slot;
}

void Server::CloseSlot(uint32 slot)
{
	_clients[slot].Close();

	if (slot > _nextClientSlot)
		_nextClientSlot = slot;
}

void Server::Tick()
{
	if (_nextClientSlot >= INVALID_SLOT)
	{
		if (_tcp.Accept(_clients[_nextClientSlot].tcp, _clients[_nextClientSlot].address))
		{
			//Connection made to new client
			uint32 slot = _nextClientSlot;
			_UpdateNextClientSlot();
			_OnClientConnected(_nextClientSlot);
		}
	}

	for (ServerClient& client : _clients)
	{
		//TCP
		while (client.tcpDigest.ReadNext(client.tcp))
		{
			Buffer<byte> message = client.tcpDigest.RetrieveMessage();
		}

		//UDP
		size_t sz;
		while (sz = _udp.Receive(client.address, _udpBuffer, 128))
		{


		}


	}
}
