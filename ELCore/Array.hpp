#pragma once

template <typename T>
class Array
{
public:
	using size_type = size_t;

protected:
	T* _elements;
	size_type _size;

	constexpr Array(T* elements, size_type size) noexcept : _elements(elements), _size(size) {}

public:
	constexpr size_type GetSize() const noexcept { return _size; }
	
	constexpr T& operator[](size_type index) noexcept { return _elements[index]; }
	constexpr const T& operator[](size_type index) const noexcept { return _elements[index]; }

	constexpr T* begin() noexcept { return _elements; }
	constexpr T* end() noexcept { return _elements + _size; }
	constexpr const T* begin() const noexcept { return _elements; }
	constexpr const T* end() const noexcept { return _elements + _size; }
};
