#include "ByteReader.hpp"
#include "Types.hpp"

bool ByteReader::SetIndex(size_t index)
{
	_index = index;

	if (_index < _buffer.GetSize())
	{
		_pointer = &_buffer[_index];
		return true;
	}

	_pointer = nullptr;
	return false;
}

bool ByteReader::IncrementIndex(size_t amount)
{
	return SetIndex(_index + amount);
}

size_t ByteReader::GetRemainingSpace()
{
	return _buffer.GetSize() == 0 ? 0 : (_buffer.GetSize() - _index);
}

size_t ByteReader::Read(byte* dest, size_t amount)
{
	for (size_t i = 0; i < amount; ++i)
	{
		dest[i] = *_pointer;

		if (!IncrementIndex(1))
			return i + 1;
	}

	return amount;
}

byte ByteReader::Peek_byte()
{
	return *_pointer;
}

byte ByteReader::Read_byte()
{
	byte value = Peek_byte();
	IncrementIndex(1);
	return value;
}

uint16 ByteReader::Peek_uint16()
{
	return (_pointer[0] << 8) + _pointer[1];
}

uint16 ByteReader::Read_uint16()
{
	uint16 value = Peek_uint16();
	IncrementIndex(2);
	return value;
}

uint16 ByteReader::Peek_uint16_little()
{
	return _pointer[0] + (_pointer[1] << 8);
}

uint16 ByteReader::Read_uint16_little()
{
	uint16 value = Peek_uint16_little();
	IncrementIndex(2);
	return value;
}

uint32 ByteReader::Peek_uint32()
{
	return (_pointer[0] << 24) + (_pointer[1] << 16) + (_pointer[2] << 8) + _pointer[3];
}

uint32 ByteReader::Read_uint32()
{
	uint16 value = Peek_uint32();
	IncrementIndex(4);
	return value;
}

uint32 ByteReader::Peek_uint32_little()
{
	return _pointer[0] + (_pointer[1] << 8) + (_pointer[2] << 16) + (_pointer[3] << 24);
}

uint32 ByteReader::Read_uint32_little()
{
	uint32 value = Peek_uint32_little();
	IncrementIndex(4);
	return value;
}

float ByteReader::Peek_float()
{
	//Todo- Proper IEEE 754 conversion

	union
	{
		//yucky stuff
		byte bytes[4];

		float value;
	};

	bytes[0] = _pointer[0];
	bytes[1] = _pointer[1];
	bytes[2] = _pointer[2];
	bytes[3] = _pointer[3];
	return value;
}

float ByteReader::Read_float()
{
	float value = Peek_float();
	IncrementIndex(4);
	return value;
}
