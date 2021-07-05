#pragma once

template <typename T>
class DefaultAllocator
{
public:
	using value_type = T;

	DefaultAllocator() noexcept {}

	template <typename O>
	DefaultAllocator(const DefaultAllocator<O>&) noexcept {}

	template <typename O>
	DefaultAllocator& operator=(const DefaultAllocator<O>& other) noexcept { return *this; }

	template <typename O>
	DefaultAllocator(DefaultAllocator<O>&&) noexcept {}

	template <typename O>
	DefaultAllocator& operator=(DefaultAllocator<O>&& other) noexcept { return *this; }

	bool operator==(const DefaultAllocator& other) const noexcept { return true; }

	constexpr value_type* allocate(size_t size) { return (value_type*)::operator new(sizeof(value_type) * size); }
	constexpr void deallocate(value_type* data, size_t size) noexcept { return ::operator delete(data); }
};

template <typename T>
class VAllocator
{
public:
	using value_type = T;

	VAllocator() noexcept {}

	VAllocator(const DefaultAllocator<T>& defaultAlloc) {}

	template <typename O>
	VAllocator(const VAllocator<O>&) noexcept {}

	template <typename O>
	VAllocator& operator=(const VAllocator<O>& other) noexcept { return *this; }

	template <typename O>
	VAllocator(VAllocator<O>&&) noexcept {}

	template <typename O>
	VAllocator& operator=(VAllocator<O>&& other) noexcept { return *this; }

	bool operator==(const VAllocator& other) const noexcept { return true; }

	virtual value_type* allocate(size_t size) { return (value_type*)::operator new(sizeof(value_type) * size); }
	virtual void deallocate(value_type* data, size_t size) noexcept { return ::operator delete(data); }
};

#include <iostream>

template <typename T>
class DebugAllocator
{
public:
	using value_type = T;

	DebugAllocator() {}

	template <typename O>
	DebugAllocator(const DebugAllocator<O>&) noexcept {}

	template <typename O>
	DebugAllocator(DebugAllocator<O>&&) noexcept {}

	value_type* allocate(size_t size)
	{
		std::cout << "allocate " << size << 'x' << typeid(T).name() << std::endl;
		return (value_type*)::operator new(sizeof(value_type) * size);
	}

	void deallocate(value_type* data, size_t size)
	{
		std::cout << "deallocate " << size << 'x' << typeid(T).name() << std::endl;
		::operator delete(data);
	}

	bool operator==(const DebugAllocator& other) const noexcept { return true; }
};
