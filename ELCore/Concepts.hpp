#pragma once
#ifdef __INTELLISENSE__
#define __cpp_lib_concepts //C'mon intellisense.. keep up!
#endif // __INTELLISENSE__

#include <concepts>
#include <functional>
#include <type_traits>

namespace Concepts
{
	template <class Derived, class Base>
	concept DerivedFrom = std::is_base_of_v<Base, Derived>;

	template<typename T>
	concept Callback = requires(const T& t) { t(); };

	template<typename T, typename RETURN, typename... Args>
	concept Function = requires(const T & t, Args&&... args) { requires std::convertible_to<decltype(t(static_cast<Args&&>(args)...)), RETURN>; };

	template<typename T, typename Arg>
	concept Predicate = Function<T, bool, Arg>;
}
