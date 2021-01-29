#include "TCPDigest.hpp"
#include "NetUtils.hpp"
#include "TCPSocket.hpp"
#include <ELSys/Debug.hpp>

uint32 TCPDigest::Read(const byte* data, uint32 size)
{
	uint32 amountRead = 0;

	while (_size == 0 && size > 0)
	{
		if (_sizeFirstByte)
		{
			_size = data[0] | ((_sizeFirstByte & 0x7F) << 8);
		}
		else
		{
			if (data[0] & 0x80)
				_sizeFirstByte = data[0];
			else
				_size = data[0];
		}

		++data;
		--size;
		++amountRead;
	}

	if (size <= 0) return _size > 0 && _size == _currentSize;

	if (_currentSize + size > _size)
		size = _size - _currentSize;

	amountRead += size;

	if (!_writer.Write(data, size) && _currentSize <= _data.STATIC_SIZE)
	{
		_writer = ByteWriter(_data.dynamicBuffer);
		_writer.Write(_data.staticBuffer, _currentSize);
		_writer.Write(data, size);
		
		if (_data.dynamicBuffer.GetSize() != _currentSize)
		{
			Debug::Error("Net message dynamic buffer size is incorrect!");
		}
	}
	
	return amountRead + size;
}

bool TCPDigest::ReadNext(const TCPSocket& tcp)
{
	if (IsComplete()) Reset();

	while (1)
	{
		while (_tcp.remaining > 0)
		{
			uint32 amount = Read(_tcp.ptr, _tcp.remaining);

			_tcp.ptr += amount;
			_tcp.remaining -= amount;

			if (IsComplete())
				return true;
		}

		_tcp.remaining = tcp.Receive(_tcp.buffer, _tcp.BUFFER_SIZE);
		if (_tcp.remaining == 0)
			return false;

		_tcp.ptr = _tcp.buffer;
	}
}

Buffer<byte> TCPDigest::RetrieveMessage() const
{
	if (IsComplete())
		return _currentSize > _data.STATIC_SIZE ? _data.dynamicBuffer : Buffer(_data.staticBuffer, _currentSize);

	return Buffer<byte>();
}

void TCPDigest::Reset()
{
	_data.dynamicBuffer.Clear();
	_currentSize = _size = _sizeFirstByte = 0;
	_writer = ByteWriter(_data.staticBuffer, _data.STATIC_SIZE);

	_tcp.remaining = 0;
}
