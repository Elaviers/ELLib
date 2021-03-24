#pragma once
#include "Concepts.hpp"
#include "Utilities.hpp"

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
	MemberFunction(RETURNTYPE(T::* function)(ARGS...)) : _isConstFunction(false), _function(function)		{}
	MemberFunction(RETURNTYPE(T::* function)(ARGS...) const) :	_isConstFunction(true), _constFunction(function)	{}

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

/*
	Function

	A reference to any static function/lambda
*/

template <typename RETURNTYPE, typename ...ARGS>
class Function
{
	class Callable
	{
	public:
		virtual ~Callable() {}

		virtual RETURNTYPE Call(ARGS...) const = 0;

		//Allocs to heap placement is nullptr
		virtual Callable* Clone(void* placement) const = 0;

		virtual bool operator==(const Callable& other) const = 0;
	};

	class BasicCallable : public Callable
	{
		RETURNTYPE (*_fptr)(ARGS...);

	public:
		BasicCallable(RETURNTYPE(*fptr)(ARGS...)) : _fptr(fptr) {}
		virtual ~BasicCallable() {}

		virtual RETURNTYPE Call(ARGS... args) const override { return _fptr(std::forward<ARGS>(args)...); }
		virtual Callable* Clone(void* placement) const override { return placement ? (new (placement) BasicCallable(_fptr)) : new BasicCallable(_fptr); }

		virtual bool operator==(const Callable& other) const override
		{
			const BasicCallable* b = dynamic_cast<const BasicCallable*>(&other);
			return b ? (_fptr == b->_fptr) : false;
		}
	};

	template <typename T>
	class InstancedCallable : public Callable
	{
		T* _obj;
		MemberFunction<T, RETURNTYPE, ARGS...> _fptr;

	public:
		InstancedCallable(T& object, RETURNTYPE(T::* function)(ARGS...)) : _obj(&object), _fptr(function) {}
		InstancedCallable(T& object, RETURNTYPE(T::* function)(ARGS...) const) : _obj(&object), _fptr(function) {}
		virtual ~InstancedCallable() {}

		virtual RETURNTYPE Call(ARGS... args) const override { return _fptr.Call(*_obj, std::forward<ARGS>(args)...); }
		virtual Callable* Clone(void* placement) const override { return placement ? (new (placement) InstancedCallable<T>(*this)) : (new InstancedCallable<T>(*this)); }

		virtual bool operator==(const Callable& other) const override
		{
			const InstancedCallable<T>* i = dynamic_cast<const InstancedCallable<T>*>(&other);
			return i ? (i->_obj == _obj && i->_fptr == _fptr) : false;
		}
	};

	template <typename T>
	class GenericCallable : public Callable
	{
		T _callable;

	public:
		GenericCallable(const T& callable) : _callable(callable) {}
		virtual ~GenericCallable() {}

		virtual RETURNTYPE Call(ARGS... args) const override { return _callable(std::forward<ARGS>(args)...); }
		virtual Callable* Clone(void* placement) const override { return placement ? (new (placement) GenericCallable(_callable)) : (new GenericCallable(_callable)); }

		virtual bool operator==(const Callable& other) const override { return false; }
	};

	/////
	alignas(InstancedCallable<BasicCallable>) byte _static[sizeof(InstancedCallable<BasicCallable>)]; //Assumes InstancedCallable<BasicCallable> is the largest any BasicCallable or InstancedCallable can be
	
	Callable* _callable;

	bool _IsAllocated() const { return (byte*)_callable != _static; }

public:
	Function() : _callable(nullptr), _static() {}
	Function(const Function& other)
	{
		if (other._IsAllocated()) _callable = other._callable ? other._callable->Clone(nullptr) : nullptr;
		else _callable = other._callable->Clone(_static);
	}

	Function(Function&& other)
	{
		if (other._IsAllocated())
		{
			_callable = other._callable;
			other._callable = nullptr;
		}
		else _callable = other._callable->Clone(_static);
	}
	
	~Function() 
	{ 
		if (_IsAllocated()) delete _callable; 
		else _callable->~Callable();
	}

	Function(RETURNTYPE(*fptr)(ARGS...)) { _callable = new (_static) BasicCallable(fptr); }

	template <typename T>
	Function(T& obj, RETURNTYPE(T::* fptr)(ARGS...)) { _callable = new (_static) InstancedCallable<T>(obj, fptr); }
	
	template <typename T>
	Function(T& obj, RETURNTYPE(T::* fptr)(ARGS...) const) { _callable = new (_static) InstancedCallable<T>(obj, fptr); }
	
	template <typename T>
	Function(const T& function) { _callable = new GenericCallable<T>(function); }

	Function& operator=(const Function& other)
	{
		if (_IsAllocated()) delete _callable;

		if (other._IsAllocated()) _callable = other._callable ? other._callable->Clone(nullptr) : nullptr;
		else _callable = other._callable->Clone(_static);

		return *this;
	}

	Function& operator=(Function&& other)
	{
		if (_IsAllocated()) delete _callable;

		if (other._IsAllocated())
			_callable = other._callable;
		else _callable = other._callable->Clone(_static);
		
		other._callable = nullptr;

		return *this;
	}
	
	RETURNTYPE operator()(ARGS ...args) const	{ return _callable->Call(std::forward<ARGS>(args)...); }

	bool IsValid() const { return _callable != nullptr; }
	void TryCall(ARGS ...args) const { if (_callable) _callable->Call(std::forward<ARGS>(args)...); }
};

typedef Function<void> Callback;

template<typename T>
using Getter = Function<T>;

template<typename T, typename RETURNTYPE>
using MemberGetter = MemberFunction<T, RETURNTYPE>;

template<typename T>
using Setter = Function<void, const T&>;

template<typename T, typename V>
using MemberSetter = MemberFunction<T, void, const V&>;

using NewHandler = Function<byte*, size_t>;

using DeleteHandler = Function<void, byte*>;

class Context;
typedef Function<void, const Buffer<String>&, const Context&> CommandPtr;

template<typename T>
using MemberCommandPtr = MemberFunction<T, void, const Buffer<String>&, const Context&>;

template<typename T>
using ContextualGetter = Function<T, const Context&>;

template<typename T, typename RETURNTYPE>
using ContextualMemberGetter = MemberFunction<T, RETURNTYPE, const Context&>;

template<typename T>
using ContextualSetter = Function<void, const T&, const Context&>;

template<typename T, typename V>
using ContextualMemberSetter = MemberFunction<T, void, const V&, const Context&>;
