#pragma once
#include "Buffer.hpp"
#include "Types.hpp"

class ByteReader
{
	const Array<byte>* _buffer;
	const byte* _rawBuffer;

	const byte* _pointer;
	size_t _index;
	size_t _bufferSize;

public:
	constexpr ByteReader(const Array<byte>& buffer, size_t index = 0) noexcept : 
		_buffer(&buffer), _rawBuffer(nullptr), _pointer(nullptr) { if (!SetIndex(index)) _index = 0; }

	constexpr ByteReader(const Buffer<byte>& buffer, size_t index = 0) noexcept : ByteReader((const Array<byte>&)buffer, index) {}

	constexpr ByteReader(const byte* data, size_t size) noexcept :
		_buffer(nullptr), _rawBuffer(data), _bufferSize(size) { SetIndex(0); }

	constexpr size_t GetIndex() const noexcept { return _index; }
	constexpr const byte* Ptr() const noexcept { return _pointer; }
	constexpr bool IsValid() const noexcept { return _index < _bufferSize; }

	constexpr bool SetIndex(size_t index);
	bool IncrementIndex(size_t amount);
	size_t GetRemainingSpace();

	//returns bytes read
	size_t Read(byte* dest, size_t amount);

	byte Peek_byte() const;
	uint16 Peek_uint16() const;
	uint16 Peek_uint16_little() const;
	uint32 Peek_uint32() const;
	uint32 Peek_uint32_little() const;
	float Peek_float() const;
	double Peek_double() const;

	byte Read_byte();
	uint16 Read_uint16();
	uint16 Read_uint16_little();
	uint32 Read_uint32();
	uint32 Read_uint32_little();
	float Read_float();
	double Read_double();

	template <typename T> T Read() { T t; t.Read(*this); return t; }
	template <> byte Read() { return Read_byte(); }
	template <> uint16 Read() { return Read_uint16(); }
	template <> uint32 Read() { return Read_uint32(); }
	template <> float Read() { return Read_float(); }
	template <> double Read() { return Read_double(); }
};
