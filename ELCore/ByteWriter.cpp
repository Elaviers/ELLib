#include "ByteWriter.hpp"
#include "IEEE754.hpp"

bool ByteWriter::SetIndex(size_t index)
{
	_index = index;

	if (_buffer) _bufferSize = _buffer->GetSize();
	if (_index < _bufferSize)
	{
		_pointer = _buffer ? &(*_buffer)[_index] : _rawBuffer + _index;
		return true;
	}

	_pointer = nullptr;
	return false;
}

bool ByteWriter::IncrementIndex(size_t amount)
{
	return SetIndex(_index + amount);
}

size_t ByteWriter::GetRemainingSpace()
{
	if (_buffer) _bufferSize = _buffer->GetSize();

	return _index > _bufferSize ? 0 : (_bufferSize - _index);
}

bool ByteWriter::EnsureSpace(size_t amount)
{
	size_t remainingSpace = GetRemainingSpace();

	if (amount > remainingSpace)
	{
		if (_buffer)
		{
			_buffer->Append(amount - remainingSpace);
			_pointer = &(*_buffer)[_index];
		}
		else
			return false;
	}

	return true;
}

bool ByteWriter::Write(const byte* data, size_t size)
{
	if (EnsureSpace(size))
	{
		Utilities::CopyBytes(data, _pointer, size);
		return true;
	}

	return false;
}

bool ByteWriter::Write_byte(byte value)
{
	if (EnsureSpace(1))
	{
		*_pointer = value;
		IncrementIndex(1);
		return true;
	}

	return false;
}

bool ByteWriter::Write_uint16(uint16 value)
{
	if (EnsureSpace(2))
	{
		_pointer[0] = (value & 0xFF00) >> 8;
		_pointer[1] = (value & 0x00FF);
		IncrementIndex(2);
		return true;
	}

	return false;
}

bool ByteWriter::Write_uint16_little(uint16 value)
{
	if (EnsureSpace(2))
	{
		_pointer[1] = (value & 0xFF00) >> 8;
		_pointer[0] = (value & 0x00FF);
		IncrementIndex(2);
		return true;
	}

	return false;
}

bool ByteWriter::Write_uint32(uint32 value)
{
	if (EnsureSpace(4))
	{
		_pointer[0] = (value & 0xFF000000) >> 24;
		_pointer[1] = (value & 0x00FF0000) >> 16;
		_pointer[2] = (value & 0x0000FF00) >> 8;
		_pointer[3] = (value & 0x000000FF);
		IncrementIndex(4);
		return true;
	}

	return false;
}

bool ByteWriter::Write_uint32_little(uint32 value)
{
	if (EnsureSpace(4))
	{
		_pointer[3] = (value & 0xFF000000) >> 24;
		_pointer[2] = (value & 0x00FF0000) >> 16;
		_pointer[1] = (value & 0x0000FF00) >> 8;
		_pointer[0] = (value & 0x000000FF);
		IncrementIndex(4);
		return true;
	}

	return false;
}

bool ByteWriter::Write_float(float value)
{
	Float_IEEE754_U u(Float_IEEE754::FromFloat(value));

	//The bytes are in little-endian order but I like things big-endian for IO
	if (EnsureSpace(4))
	{
		_pointer[0] = u.bytes[3];
		_pointer[1] = u.bytes[2];
		_pointer[2] = u.bytes[1];
		_pointer[3] = u.bytes[0];
		IncrementIndex(4);
		return true;
	}

	return false;
}

bool ByteWriter::Write_cstr(const char* string)
{
	size_t len = StringLength(string);

	if (EnsureSpace(len + 1))
	{
		_pointer[len] = '\0';

		for (unsigned int i = 0; i < len; ++i)
			_pointer[i] = string[i];

		IncrementIndex(len + 1);
		return true;
	}

	return false;
}
