#pragma once
#include "Concepts.hpp"

namespace IteratorUtils
{
	template <typename IT, typename P>
	requires Concepts::Predicate<P, const IT&>
	constexpr IT FirstWhere(IT begin, const IT& end, const P& predicate) noexcept
	{
		while (begin != end && !predicate(begin))
			++begin;

		return begin;
	}

	template <typename IT, typename T>
	constexpr IT FirstEqualPosition(IT begin, const IT& end, const T& value) noexcept
	{
		while (begin != end && *begin != value)
			++begin;

		return begin;
	}

	template <typename IT, typename T>
	constexpr IT FirstGreaterPosition(IT begin, const IT& end, const T& value) noexcept
	{
		while (begin != end && *begin < value)
			++begin;

		return begin;
	}

	template <typename IT>
	constexpr IT Offset(IT begin, size_t offset) noexcept
	{
		for (size_t i = 0; i < offset; ++i)
			++begin;

		return begin;
	}

	template <typename IT>
	constexpr IT Offset(IT begin, const IT& end, size_t offset) noexcept
	{
		for (size_t i = 0; i < offset && begin != end; ++i)
			++begin;

		return begin;
	}

	constexpr const size_t INVALID_INDEX = ~((size_t)0);

	template <typename IT, typename T>
	constexpr size_t IndexOf(IT begin, const IT& end, const T& value) noexcept
	{
		size_t index = 0;
		for (; *begin != value; ++index, ++begin)
			if (begin == end)
				return INVALID_INDEX;

		return index;
	}

	template <typename IT, typename T>
	constexpr size_t IndexOf(IT begin, const IT& end, const T& value, size_t startIndex) noexcept
	{
		begin = Offset(begin, end, startIndex);
		size_t index = startIndex;
		for (; *begin != value; ++index, ++begin)
			if (begin == end)
				return INVALID_INDEX;

		return index;
	}

	template <typename IT, typename T>
	constexpr size_t IndexOfRev(const IT& begin, IT end, const T& value) noexcept
	{
		size_t index = 0;
		for (; *end != value; --end)
			if (end == begin)
				return INVALID_INDEX;

		for (; end != begin; --end, ++index);

		return index;
	}
}
