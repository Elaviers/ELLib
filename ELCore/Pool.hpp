#pragma once
#include "Types.hpp"
#include <utility>

template <typename T, int SIZE>
class Pool
{
	byte _data[SIZE * sizeof(T)];
	size_t _usedSlotCount;

	T& GetElement(size_t index) { return (T&)_data[index * sizeof(T)]; }

public:
	Pool() : _data(), _usedSlotCount(0) {}

	~Pool()
	{
		for (size_t i = 0; i < _usedSlotCount; ++i)
			GetElement(i).~T();
	}

	void Clear()
	{
		for (size_t i = 0; i < _usedSlotCount; ++i)
			GetElement(i).~T();

		_usedSlotCount = 0;
	}

	bool IsFull() const { return _usedSlotCount >= SIZE; }
	size_t GetUsedSlotCount() const { return _usedSlotCount; }

	template <typename... ARGS>
	T* New(ARGS&&... args)
	{
		return _usedSlotCount < SIZE ? new(_data + (_usedSlotCount++ * sizeof(T))) T(std::forward(args)...) : nullptr;
	}

	T* NewArray(size_t size)
	{
		if (size == 0 || _usedSlotCount + size > SIZE) return nullptr;

		T* mem = new(_data + (_usedSlotCount * sizeof(T))) T[size];
		_usedSlotCount += size;
		return mem;
	}

	void DeleteHandler(T*) const {}
};
