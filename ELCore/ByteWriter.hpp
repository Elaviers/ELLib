#pragma once
#include "Buffer.hpp"

class ByteWriter
{
	Buffer<byte>* _buffer;
	byte* _rawBuffer;

	byte* _pointer;
	size_t _index;
	size_t _bufferSize;

public:
	ByteWriter(Buffer<byte>& buffer, size_t index = 0) : 
		_buffer(&buffer), _rawBuffer(nullptr), _pointer(nullptr) { if (!SetIndex(index)) _index = 0; }

	ByteWriter(byte* data, size_t size) :
		_buffer(nullptr), _rawBuffer(data), _bufferSize(size) { SetIndex(0); }

	size_t GetIndex() const { return _index; }
	byte* Ptr() { return _pointer; }
	bool IsValid() const { return _index < _bufferSize; }

	bool SetIndex(size_t index);
	bool IncrementIndex(size_t amount);
	size_t GetRemainingSpace();

	bool EnsureSpace(size_t amount);
	
	bool Write(const byte* data, size_t size);
	
	bool Write_byte(byte value);
	bool Write_uint16(uint16 value);
	bool Write_uint16_little(uint16 value);
	bool Write_uint32(uint32 value);
	bool Write_uint32_little(uint32 value);
	bool Write_float(float value);
	bool Write_double(double value);
	bool Write_cstr(const char* string);

	//todo: "return true"... really..?
	template <typename T> bool Write(T t) { t.Write(*this); return true; }
	template <> bool Write(byte x) { return Write_byte(x); }
	template <> bool Write(uint16 x) { return Write_uint16(x); }
	template <> bool Write(uint32 x) { return Write_uint32(x); }
	template <> bool Write(float x) { return Write_float(x); }
	template <> bool Write(double x) { return Write_double(x); }
	template <> bool Write(const char* x) { return Write_cstr(x); }
};

