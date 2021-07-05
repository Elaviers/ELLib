#pragma once

template<typename T, typename RETURNTYPE, typename ...ARGS>
class MemberFunction
{
private:
	union
	{
		RETURNTYPE(T::* _function)(ARGS...);
		RETURNTYPE(T::* _constFunction)(ARGS...) const;

		void* _voidPtr;
	};

	const bool _isConstFunction;

public:
	MemberFunction() : _isConstFunction(false), _function(nullptr) {}
	MemberFunction(RETURNTYPE(T::* function)(ARGS...)) : _isConstFunction(false), _function(function) {}
	MemberFunction(RETURNTYPE(T::* function)(ARGS...) const) : _isConstFunction(true), _constFunction(function) {}

	RETURNTYPE Call(T& obj, ARGS ...args) const
	{
		if (_isConstFunction)
			return (obj.*_constFunction)(std::forward<ARGS>(args)...);

		return (obj.*_function)(std::forward<ARGS>(args)...);
	}

	RETURNTYPE Call(const T& obj, ARGS ...args) const
	{
		//todo: not this
		return (obj.*_constFunction)(std::forward<ARGS>(args)...);
	}

	void* GetRawPointer() const { return _voidPtr; }

	bool operator==(const MemberFunction& other) const { return _voidPtr == other._voidPtr && _isConstFunction == other._isConstFunction; }
	bool operator!=(const MemberFunction& other) const { return _voidPtr != other._voidPtr || _isConstFunction != other._isConstFunction; }
};

template<typename T, typename RETURNTYPE>
using MemberGetter = MemberFunction<T, RETURNTYPE>;

template<typename T, typename V>
using MemberSetter = MemberFunction<T, void, const V&>;

template <typename T>
class Array;

class Context;

#include "String.hpp"

template<typename T>
using MemberCommandPtr = MemberFunction<T, void, const Array<String>&, const Context&>;

template<typename T, typename RETURNTYPE>
using ContextualMemberGetter = MemberFunction<T, RETURNTYPE, const Context&>;

template<typename T, typename V>
using ContextualMemberSetter = MemberFunction<T, void, const V&, const Context&>;
