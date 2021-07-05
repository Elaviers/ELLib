#pragma once
#include "Allocator.hpp"
#include "Array.hpp"
#include <initializer_list>

template <typename T, typename AllocatorType = DefaultAllocator<T>>
class Buffer
{
public:
	using size_type = size_t;
	using value_type = T;

private:
	template <typename _T, typename _Alloc>
	friend class Buffer;

	class Proxy : public AllocatorType, public Array<T>
	{
		friend Buffer;

	public:
		constexpr Proxy() noexcept : Array<T>(nullptr, 0) {}
		constexpr Proxy(const AllocatorType& allocator) noexcept : AllocatorType(allocator), Array<T>(nullptr, 0) {}

		constexpr Proxy(const Proxy& other) : AllocatorType((const AllocatorType&)other), Array<T>(nullptr, other._size) {}
		
		constexpr Proxy(Proxy&& other) noexcept : AllocatorType(std::move((AllocatorType&)other)), Array<T>(other._elements, other._size)
		{
			other._elements = nullptr;
			other._size = 0;
		}

		constexpr Proxy& operator=(const Proxy& other) 
		{
			AllocatorType::operator=((const AllocatorType&)other);
			this->_elements = nullptr;
			this->_size = other._size;
			return *this;
		}

		constexpr Proxy& operator=(Proxy&& other) noexcept
		{
			AllocatorType::operator=(std::move((AllocatorType&)other));
			this->_elements = other._elements;
			this->_size = other._size;
			other._elements = nullptr;
			other._size = 0;
			return *this;
		}
	} _proxy;

	T*& _elements = _proxy._elements;
	size_type& _size = _proxy._size;

	constexpr void _DestroyElements()
	{
		for (size_type i = 0; i < _size; ++i)
			_elements[i].~T();

		_proxy.deallocate(_elements, _size);
	}

public:
	constexpr Buffer() noexcept {}

	constexpr Buffer(const AllocatorType& allocator) noexcept : _proxy(allocator) {}
	
	constexpr Buffer(const std::initializer_list<T>& elements, const AllocatorType& allocator = AllocatorType()) :
		_proxy(allocator)
	{
		if (_size = elements.size())
		{
			_elements = _proxy.allocate(_size);

			size_type i = 0;
			for (const T& elem : elements)
				new (_elements + i++) T(elem);
		}
	}

	constexpr Buffer(const T* elements, size_type size, const AllocatorType& allocator = AllocatorType()) : _proxy(allocator)
	{
		if (_size = size)
		{
			_elements = _proxy.allocate(_size);

			for (size_type i = 0; i < _size; ++i)
				new (_elements + i) T(elements[i]);
		}
	}

	constexpr Buffer(const Array<T>& array, const AllocatorType& allocator = AllocatorType()) : Buffer(array.begin(), array.GetSize(), allocator) {}

	constexpr Buffer(const Buffer& other) : _proxy(other._proxy)
	{
		if (_size)
		{
			_elements = _proxy.allocate(_size);
			for (size_type i = 0; i < _size; ++i)
				new (_elements + i) T(other._elements[i]);
		}
	}

	constexpr Buffer(Buffer&& other) noexcept : _proxy(std::move(other._proxy)) {}

	constexpr ~Buffer() noexcept
	{
		_DestroyElements();
	}

	constexpr Buffer& operator=(const Buffer& other)
	{
		_proxy = other._proxy;
		if (_size)
		{
			_elements = _proxy.allocate(_size);
			for (size_type i = 0; i < _size; ++i)
				new (_elements + i) T(other._elements[i]);
		}

		return *this;
	}

	constexpr Buffer& operator=(Buffer&& other) noexcept
	{
		_proxy = std::move(other._proxy);
		return *this;
	}

	constexpr operator Array<T>&() noexcept
	{
		return _proxy;
	}

	constexpr operator const Array<T>&() const noexcept
	{
		return _proxy;
	}

	/*
		SIZE
	*/

	constexpr size_type GetSize() const noexcept { return _size; }

	constexpr void Clear()
	{
		_DestroyElements();
		_elements = nullptr;
		_size = 0;
	}

	constexpr void Shrink(size_type amount)
	{
		if (amount)
		{
			if (amount >= _size)
				return Clear();

			size_type newSize = _size - amount;
			T* newElements = _proxy.allocate(newSize);

			for (size_type i = 0; i < newSize; ++i)
				new (newElements + i) T(std::move(_elements[i]));

			_DestroyElements();
			_elements = newElements;
			_size = newSize;
		}
	}

	template <typename... Args>
	constexpr void Grow(size_type amount, Args&&... defaultCtorArgs)
	{
		if (amount)
		{
			size_type newSize = _size + amount;
			T* newElements = _proxy.allocate(newSize);

			for (size_type i = 0; i < _size; ++i)
				new (newElements + i) T(std::move(_elements[i]));

			for (size_type i = _size; i < newSize; ++i)
				new (newElements + i) T(std::forward<Args>(defaultCtorArgs)...);

			_DestroyElements();
			_elements = newElements;
			_size = newSize;
		}
	}

	template <typename... Args>
	constexpr void SetSize(size_type size, Args&&... defaultCtorArgs)
	{
		if (size > _size) Grow(size - _size, std::forward<Args>(defaultCtorArgs)...);
		else if (size < _size) Shrink(_size - size);
	}

	/*
		INSERTION
	*/

	template <typename... Args>
	constexpr T& Insert(size_type index, Args&&... ctorArgs)
	{
		T* newElements = _proxy.allocate(_size + 1);

		for (size_type i = 0; i < index; ++i)
			new (newElements + i) T(std::move(_elements[i]));

		for (size_type i = index; i < _size; ++i)
			new (newElements + i + 1) T(std::move(_elements[i]));

		new (newElements + index) T(std::forward<Args>(ctorArgs)...);

		_proxy.deallocate(_elements, _size);
		_elements = newElements;
		++_size;

		return _elements[_size - 1];
	}

	template <typename... Args>
	constexpr T& Emplace(Args&&... ctorArgs)
	{
		T* newElements = _proxy.allocate(_size + 1);
		for (size_type i = 0; i < _size; ++i)
			new (newElements + i) T(std::move(_elements[i]));

		_DestroyElements();
		_elements = newElements;

		T* element = new (_elements + _size) T(std::forward<Args>(ctorArgs)...);
		++_size;
		return *element;
	}

	constexpr T& Add(const T& element)
	{
		return Emplace(element);
	}

	constexpr T& Add(T&& element)
	{
		return Emplace(std::move(element));
	}

	constexpr Buffer& AddMultiple(const T* elements, size_type count)
	{
		if (count)
		{
			size_type newSize = _size + count;
			T* newElements = _proxy.allocate(newSize);

			for (size_type i = 0; i < _size; ++i)
				new (newElements + i) T(std::move(_elements[i]));

			T* firstAddr = newElements + _size;
			for (size_type i = 0; i < count; ++i)
				new (firstAddr + i) T(elements[i]);

			_DestroyElements();
			_elements = newElements;
			_size = newSize;
		}

		return *this;
	}

	constexpr Buffer& AddMultiple(const Buffer& other)
	{
		AddMultiple(other._elements, other._size);
		return *this;
	}

	constexpr Buffer& AddMultiple(Buffer&& other)
	{
		if (other._size)
		{
			size_type newSize = _size + other._size;
			T* newElements = _proxy.allocate(newSize);

			for (size_type i = 0; i < _size; ++i)
				new (newElements + i) T(std::move(_elements[i]));

			T* firstAddr = newElements + _size;
			for (size_type i = 0; i < other._size; ++i)
				new (firstAddr + i) T(std::move(other._elements[i]));

			other.Clear();
			_DestroyElements();
			_elements = newElements;
			_size = newSize;
		}

		return *this;
	}

	/*
		REMOVAL
	*/

	constexpr void Remove(size_type from, size_type to)
	{
		if (from > to)
			std::swap(from, to);

		if (from > _size) return;
		if (to > _size)
		{
			Shrink(_size - from);
			return;
		}

		size_type elementsRemoved = to - from;
		size_type newSize = _size - elementsRemoved;
		T* newElements = _proxy.allocate(newSize);

		for (size_type i = 0; i < from; ++i)
			new (newElements + i) T(std::move(_elements[i]));

		for (size_type i = to; i < _size; ++i)
			new (newElements + i - elementsRemoved) T(std::move(_elements[i]));

		_DestroyElements();
		_elements = newElements;
		_size = newSize;
	}

	constexpr void Remove(size_type index)
	{
		if (index > _size) return;

		T* newElements = _proxy.allocate(_size + 1);

		for (size_type i = 0; i < index; ++i)
			new (newElements + i) T(std::move(_elements[i]));

		for (size_type i = index + 1; i < _size; ++i)
			new (newElements + i - 1) T(std::move(_elements[i]));

		_DestroyElements();
		_elements = newElements;
		--_size;
	}

	/*
		MEMBER ACCESS
	*/

	constexpr T& operator[](size_type index) { return _elements[index]; }
	constexpr const T& operator[](size_type index) const { return _elements[index]; }

	constexpr size_type PositionToIndex(const T* position) const noexcept { return position - begin(); }

	/*
		ITERATION
	*/

	constexpr T* begin() noexcept { return _elements; }
	constexpr const T* begin() const noexcept { return _elements; }
	constexpr T* end() noexcept { return _elements ? (_elements + _size) : nullptr; }
	constexpr const T* end() const noexcept { return _elements ? (_elements + _size) : nullptr; }

	/*
		OTHER
	*/

	constexpr bool Matches(const Buffer& other) const
	{
		if (other.GetSize() != _size) return false;

		for (size_type i = 0; i < _size; ++i)
			if (_elements[i] != other._elements[i])
				return false;

		return true;
	}
};

template <typename T>
using VBuffer = Buffer<T, VAllocator<T>>;
