#pragma once
#include <ELCore/Buffer.hpp>
#include <ELCore/ByteReader.hpp>
#include <ELCore/ByteWriter.hpp>
#include <ELCore/Types.hpp>

class TCPDigest
{
private:
	//Message data
	//This does not include the size information
	struct _Data
	{
		static constexpr const int STATIC_SIZE = 32;
		byte staticBuffer[STATIC_SIZE];
		Buffer<byte> dynamicBuffer;
	} _data;

	byte _sizeFirstByte;
	uint16 _currentSize;	
	uint16 _size;
	
	ByteWriter _writer;

	struct _TCP
	{
		static constexpr const int BUFFER_SIZE = 32;
		byte buffer[BUFFER_SIZE];
		byte* ptr = nullptr;
		uint32 remaining = 0;
	} _tcp;

public:
	TCPDigest() : 
		_currentSize(0), _sizeFirstByte(0), _size(0), 
		_writer(_data.staticBuffer, _Data::STATIC_SIZE) {}

	/*
		Read

		Reads bytes into the message, stopping when completed
		Returns the amount of bytes that were used used (less than size if the message was complete)
	*/
	uint32 Read(const byte* data, uint32 size);

	/*
		ReadNext

		Attempts to complete a message using data from a tcp socket
		Note that the reader will be reset if there is currently a completed message in it
	*/
	bool ReadNext(const class TCPSocket& tcp);

	bool IsComplete() const { return _currentSize == _size; }

	Buffer<byte> RetrieveMessage() const;

	void Reset();
};
