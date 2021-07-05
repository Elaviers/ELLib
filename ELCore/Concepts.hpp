#pragma once
#include <concepts>
#include <functional>
#include <type_traits>

namespace Concepts
{
	template <class Derived, class Base>
	concept DerivedFrom = std::is_base_of_v<Base, Derived>;

	template<typename T>
	concept Callback = requires(const T& t) { t(); };

	template <typename T, typename ReturnType, typename... Args>
	concept Function = requires(const T & func, Args ...args)
	{
		{ func(args...) } -> std::convertible_to<ReturnType>;
	};

	template<typename T, typename Arg>
	concept Predicate = Function<T, bool, Arg>;
}
