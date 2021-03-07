#pragma once
#include "FunctionPointer.hpp"
#include "Types.hpp"
#include "Utilities.hpp"
#include <initializer_list>
#include <utility>

/*
	Basically just a dynamically sized array
	Always guaranteed to be continous in memory
*/

template <typename T>
class Buffer
{
private:
	const NewHandler _handlerNew;
	const DeleteHandler _handlerDelete;

	union
	{
		T* _elements;
		byte* _raw;
	};

	size_t _size;

	byte* _AllocRawData(size_t newSize) const
	{
		if (_handlerNew.IsCallable())
			return _handlerNew(sizeof(T) * newSize);

		return new byte[sizeof(T) * newSize];
	}

	void _DestroyData()
	{
		for (size_t i = 0; i < _size; ++i)
			_elements[i].~T();

		if (_handlerDelete.IsCallable())
			_handlerDelete(_raw);
		else
			delete[] _raw;
	}

	template <typename S>
	void _Quicksort(size_t first, size_t last, S(*func)(const T&))
	{
		if (first >= last)
			return;

		const S pivot = func(_elements[(first + last) / 2]);

		size_t i = first, j = last;
		while (true)
		{
			while (func(_elements[i]) < pivot)
				++i;

			while (func(_elements[j]) > pivot)
				--j;

			if (i >= j)
				break;

			Utilities::Swap(_elements[i], _elements[j]);
			++i; --j;
		}

		_Quicksort(first, j, func);
		_Quicksort(j + 1, last, func);
	}

public:
	Buffer() : _raw(nullptr), _size(0) {}
	Buffer(const NewHandler& newHandler, const DeleteHandler& deleteHandler) : Buffer(), _handlerNew(newHandler), _handlerDelete(deleteHandler) {}

	Buffer(const std::initializer_list<T>& elements) : _size(elements.size())
	{
		_raw = _AllocRawData(_size);

		for (size_t i = 0; i < _size; ++i)
			new (_elements + i) T(std::move(elements.begin()[i]));
	}

	Buffer(const T* data, size_t size) : _size(size)
	{
		_raw = _AllocRawData(_size);

		for (size_t i = 0; i < _size; ++i)
			new (_elements + i) T(data[i]);
	}

	Buffer(const Buffer& other) : _size(other._size)
	{
		_raw = _AllocRawData(_size);

		for (size_t i = 0; i < _size; ++i)
			new (_elements + i) T(other[i]);
	}

	Buffer(Buffer&& other) noexcept
	{
		operator=(std::move(other));
	}

	~Buffer()
	{
		_DestroyData();
	}

	size_t GetSize() const { return _size; }
	const T* Elements() const { return _elements; }
	T* Elements() { return _elements; }

	T& operator[](size_t index) { return _elements[index]; }
	const T& operator[](size_t index) const { return _elements[index]; }

	T& Last() { return _elements[_size - 1]; }
	const T& Last() const { return _elements[_size - 1]; }

	void SetSize(size_t size)
	{
		if (size == _size) return;
		if (size > _size) return Grow(size - _size);
		return Shrink(_size - size);
	}

	void Grow(size_t amount)
	{
		if (amount > 0)
		{
			size_t size = _size + amount;
			byte* newData = _AllocRawData(size);
			for (size_t i = 0; i < _size; ++i)
				new (newData + sizeof(T) * i) T(std::move(_elements[i]));

			byte* newDataStart = newData + _size * sizeof(T);
			for (size_t i = 0; i < amount; ++i)
				new (newDataStart + sizeof(T) * i) T();

			_DestroyData();
			_raw = newData;
			_size = size;
		}
	}

	void Shrink(size_t amount)
	{
		if (amount > 0)
		{
			if (amount < _size)
			{
				size_t size = _size - amount;
				byte* newData = _AllocRawData(size);

				for (size_t i = 0; i < size; ++i)
					new (newData + sizeof(T) * i) T(std::move(_elements[i]));

				_DestroyData();
				_raw = newData;
				_size = size;
			}
			else
				Clear();
		}
	}

	template <typename... Args>
	T& Emplace(Args&&... args)
	{
		byte* newData = _AllocRawData(_size + 1);
		for (size_t i = 0; i < _size; ++i)
			new (newData + sizeof(T) * i) T(std::move(_elements[i]));

		_DestroyData();
		_raw = newData;
		++_size;
		return *new (_elements + _size - 1) T(static_cast<Args&&>(args)...);
	}

	T& Add(const T& item) { return Emplace(item); }
	T& Add(T&& item) { return Emplace(std::move(item)); }

	void Clear()
	{
		_DestroyData();
		_raw = nullptr;
		_size = 0;
	}

	T* Insert(const T& item, size_t pos) { return pos > _size ? nullptr : Insert(std::move(T(item)), pos); }
	T* Insert(T&& item, size_t pos)
	{
		if (pos > _size) return nullptr;

		byte* newData = _AllocRawData(_size + 1);
		for (size_t i = 0; i < pos; ++i)
			new (newData + sizeof(T) * i) T(std::move(_elements[i]));

		new (newData + sizeof(T) * pos) T(std::move(item));

		for (size_t i = pos; i < _size; ++i)
			new (newData + sizeof(T) * (i + 1)) T(std::move(_elements[i]));

		_DestroyData();
		_raw = newData;
		_size++;

		return &_elements[pos];
	}

	T& OrderedAdd(const T& item) { return OrderedAdd(std::move(T(item))); }
	T& OrderedAdd(T&& item)
	{
		for (size_t i = 0; i < _size; ++i)
			if (_elements[i] > item)
				return *Insert(std::move(item), i);

		return Add(std::move(item));
	}

	void RemoveIndex(size_t index)
	{
		if (index < _size)
		{
			_size--;
			_elements[index].~T();

			byte* newData = _AllocRawData(_size);

			for (size_t i = 0; i < index; ++i)
				new (newData + sizeof(T) * i) T(std::move(_elements[i]));

			for (size_t i = index; i < _size; ++i)
				new (newData + sizeof(T) * i) T(std::move(_elements[i + 1]));

			_DestroyData();
			_raw = newData;
		}
	}

	//Removes anything equivalent to item from the array
	void Remove(const T& item)
	{
		for (size_t i = 0; i < _size;)
		{
			if (_elements[i] == item)
				RemoveIndex(i);
			else
				++i;
		}
	}

	Buffer operator+(const T& other) { return operator+(std::move(T(other))); }
	Buffer operator+(T&& other)
	{
		Buffer result;
		result._size = _size + 1;
		result._raw = _AllocRawData(result._size);

		for (size_t i = 0; i < _size; ++i)
			new (result._elements + i) T(_elements[i]);

		new (result._elements + _size) T(std::move(other));
		return result;
	}

	Buffer operator+(const Buffer& other) const
	{
		Buffer result;
		result._size = _size + other._size;
		result._raw = _AllocRawData(result._size);

		for (size_t i = 0; i < _size; ++i)
			new (result._elements + i) T(_elements[i]);

		for (size_t i = 0; i < other._size; ++i)
			new (result._elements + _size + i) T(other._elements[i]);

		return result;
	}

	Buffer& operator+=(const Buffer& other)
	{
		return *this = *this + other;
	}

	Buffer& operator=(const Buffer& other)
	{
		_DestroyData();
		_size = other._size;
		_raw = _AllocRawData(_size);

		for (size_t i = 0; i < _size; ++i)
			new (_elements + i) T(other[i]);

		return *this;
	}

	Buffer& operator=(Buffer&& other) noexcept
	{
		if (other._handlerNew != _handlerNew || other._handlerDelete != _handlerDelete)
		{
			operator=((const Buffer&)other);
			other.Clear();
			return *this;
		}

		_raw = other._raw;
		_size = other._size;
		other._raw = nullptr;
		other._size = 0;
		return *this;
	}

	bool operator==(const Buffer& other) const
	{
		if (_size != other._size) return false;

		for (size_t i = 0; i < _size; ++i)
			if (_elements[i] != other[i])
				return false;

		return true;
	}

	int IndexOfFirst(const T& item) const
	{
		for (size_t i = 0; i < _size; ++i)
			if (_elements[i] == item)
				return (int)i;

		return -1;
	}

	//For range-based for
	T* begin() { return _size ? _elements : nullptr; }
	const T* begin() const { return _size ? _elements : nullptr; }
	T* end() { return _size ? (_elements + _size) : nullptr; }
	const T* end() const { return _size ? (_elements + _size) : nullptr; }

	template <typename S>
	void Sort(S(*func)(const T&))
	{
		if (_size >= 2)
			_Quicksort(0, _size - 1, func);
	}
};
