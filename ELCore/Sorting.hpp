#pragma once
#include "Utilities.hpp"

namespace Sorting
{
	template <typename T, typename S>
	void Quicksort(T* elements, size_t first, size_t last, S(*func)(const T&))
	{
		if (first >= last)
			return;

		const S pivot = func(elements[(first + last) / 2]);

		size_t i = first, j = last;
		while (true)
		{
			while (func(elements[i]) < pivot)
				++i;

			while (func(elements[j]) > pivot)
				--j;

			if (i >= j)
				break;

			Utilities::Swap(elements[i], elements[j]);
			++i; --j;
		}

		Quicksort(elements, first, j, func);
		Quicksort(elements, j + 1, last, func);
	}
}
