#pragma once
#include <ELCore/Buffer.hpp>
#include "Address.hpp"
#include "TCPSocket.hpp"
#include "TCPDigest.hpp"
#include "UDPSocket.hpp"

class Server
{
private:
	class ServerClient
	{
	public:
		bool isLocalClient = false;

		Address address;

		TCPSocket tcp;
		TCPDigest tcpDigest;

		void Close();
	};

	TCPSocket _tcp;
	Buffer<ServerClient> _clients;

	static constexpr const uint32 INVALID_SLOT = 0xFFFFFFFF;
	uint32 _nextClientSlot;

	UDPSocket _udp;

	//todo: maybe not have this as a constant
	constexpr static const uint16 _MAX_UDP_PACKET_SIZE = 512;
	byte _udpBuffer[_MAX_UDP_PACKET_SIZE];

	void _UpdateNextClientSlot();

protected:
	void _OnClientConnected(uint32 slot);

public:
	Server() : _nextClientSlot(0) {}

	bool Open(ushort listenPort);
	void Close();

	//Set amount of slots in this server (must be at least 1...)
	void SetSize(uint32 size);

	uint32 OpenSlotLocally();
	void CloseSlot(uint32 slot);

	void Tick();
};
