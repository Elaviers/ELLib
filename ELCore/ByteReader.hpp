#pragma once
#include "Buffer.hpp"

class ByteReader
{
	const Buffer<byte>* _buffer;
	const byte* _rawBuffer;

	const byte* _pointer;
	size_t _index;
	size_t _bufferSize;

public:
	ByteReader(const Buffer<byte>& buffer, size_t index = 0) : 
		_buffer(&buffer), _rawBuffer(nullptr), _pointer(nullptr) { if (!SetIndex(index)) _index = 0; }

	ByteReader(const byte* data, size_t size) :
		_buffer(nullptr), _rawBuffer(data), _bufferSize(size) { SetIndex(0); }

	size_t GetIndex() const { return _index; }
	const byte* Ptr() const { return _pointer; }
	bool IsValid() const { return _index < _bufferSize; }

	bool SetIndex(size_t index);
	bool IncrementIndex(size_t amount);
	size_t GetRemainingSpace();

	//returns bytes read
	size_t Read(byte* dest, size_t amount);

	byte Peek_byte();
	uint16 Peek_uint16();
	uint16 Peek_uint16_little();
	uint32 Peek_uint32();
	uint32 Peek_uint32_little();
	float Peek_float();

	byte Read_byte();
	uint16 Read_uint16();
	uint16 Read_uint16_little();
	uint32 Read_uint32();
	uint32 Read_uint32_little();
	float Read_float();

	template <typename T> T Read() { T t; t.Read(*this); return t; }
	template <> byte Read() { return Read_byte(); }
	template <> uint16 Read() { return Read_uint16(); }
	template <> uint32 Read() { return Read_uint32(); }
	template <> float Read() { return Read_float(); }
};
