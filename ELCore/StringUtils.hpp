#pragma once

namespace StringUtils
{
	constexpr const char* ALPHA = "0123456789ABCDEFGHIJKLMNOP";

	template <typename C>
	constexpr C LowerChar(C c) noexcept
	{
		if (c <= 'Z' && c >= 'A')
			return c + ('a' - 'A');

		return c;
	}

	constexpr bool IsPartOfToken(char c, const char* delimiters, size_t delimiterCount)
	{
		if (c == '\0')
			return false;

		for (size_t i = 0; i < delimiterCount; ++i)
			if (c == delimiters[i])
				return false;

		return true;
	}

	template <typename C>
	constexpr bool NotEqual(const C* a, const C* b)
	{
		for (;; ++a, ++b)
		{
			if (*a != *b)
				return true;

			if (*a == '\0')
				return false;
		}
	}

	template <typename C>
	constexpr bool Equal(const C* a, const C* b) noexcept
	{
		return !StringUtils::NotEqual(a, b);
	}

	template <typename C>
	constexpr bool Equal_CaseInsensitive(const C* a, const C* b) noexcept
	{
		for (; ; ++a, ++b)
		{
			if (LowerChar(*a) != LowerChar(*b))
				return false;

			if (*a == '\0')
				return true;
		}
	}

	template <typename C>
	constexpr bool Contains(const C* string, const C* phrase) noexcept
	{
		for (; string[0] != '\0'; ++string)
		{
			for (size_t i = 0; ; ++i)
			{
				if (phrase[i] == '\0')
					return true;

				if (string[i] == '\0')
					return false;

				if (string[i] != phrase[i])
				{
					if (i > 1) string += i - 1;
					break;
				}
			}
		}

		return false;
	}

	template <typename C>
	constexpr bool NotEqual(const C* a, const C* b, size_t n) noexcept
	{
		for (int i = 0; i < n; ++i)
		{
			if (a[i] != b[i])
				return true;

			if (a[i] == '\0')
				return true;  //Consider strings inequal if we get to the end before n is reached
		}

		return false;
	}

	template <typename C>
	constexpr bool Equal(const C* a, const C* b, size_t n) noexcept
	{
		return !NotEqual(a, b, n);
	}

	template <typename C>
	constexpr size_t Length(const C* string) noexcept {
		size_t length = 0;
		while (string[length] != '\0') ++length;
		return length;
	}
}
