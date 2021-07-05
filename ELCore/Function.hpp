#pragma once
#include "Concepts.hpp"
#include "Utilities.hpp"

/*
	Function

	A reference to any static function/lambda
*/

template <typename ReturnType, typename... Args>
class Function
{
	class ClosureContainerBase
	{
	public:
		virtual ~ClosureContainerBase() {}
		virtual ClosureContainerBase* Clone() const = 0;
		virtual ClosureContainerBase* Clone(void* placement) const = 0;
		virtual ReturnType operator()(Args... args) const = 0;
	};

	template <typename L>
	class ClosureContainer : public ClosureContainerBase
	{
		L _closure;

	public:
		template <typename... CtorArgs>
		ClosureContainer(CtorArgs&&... args) : _closure(std::forward<CtorArgs>(args)...) {}

		virtual ~ClosureContainer() {}
		virtual ClosureContainerBase* Clone() const override { return new ClosureContainer<L>(_closure); }
		virtual ClosureContainerBase* Clone(void* placement) const override { return new (placement) ClosureContainer<L>(_closure); }
		virtual ReturnType operator()(Args... args) const override { return _closure(std::forward<Args>(args)...); }
	};

	template <typename T>
	class SimpleClosure
	{
		ReturnType(T::* _fptr)(Args...);
		T* _instance;

	public:
		SimpleClosure(ReturnType(T::* fptr)(Args...), T* instance) : _fptr(fptr), _instance(instance) {}
		ReturnType operator()(Args... args) const { return (_instance->*_fptr)(std::forward<Args>(args)...); }
	};

	template <typename T>
	class SimpleClosureConst
	{
		ReturnType(T::* _fptr)(Args...) const;
		const T* _instance;

	public:
		SimpleClosureConst(ReturnType(T::* fptr)(Args...) const, const T* instance) : _fptr(fptr), _instance(instance) {}
		ReturnType operator()(Args... args) const { return (_instance->*_fptr)(std::forward<Args>(args)...); }
	};

	ClosureContainerBase* _cc;
	ReturnType(*_fptr)(Args...);

	//Static memory for simple closures (so we can avoid heap allocations)
	//Note that this assumes that SimpleClosure<ClosureContainerBase> is the biggest SimpleClosure possible!
	using LargestSimpleClosureContainer = ClosureContainer<SimpleClosure<ClosureContainerBase>>;
	alignas(LargestSimpleClosureContainer) byte _static[sizeof(LargestSimpleClosureContainer)];

	//Does not make cc nullptr!
	void _DeleteCC()
	{
		if (_cc == (void*)_static)
		{
			_cc->~ClosureContainerBase();
		}
		else
		{
			delete _cc;
		}
	}

public:
	Function(ReturnType(*fptr)(Args...) = nullptr) : _cc(nullptr), _fptr(fptr)
	{}

	template <typename C>
	Function(ReturnType(C::* fptr)(Args...), C& instance)
	{
		_cc = new (_static) ClosureContainer<SimpleClosure<C>>(fptr, &instance);
	}

	template <typename C>
	Function(ReturnType(C::* fptr)(Args...) const, const C& instance)
	{
		_cc = new (_static) ClosureContainer<SimpleClosureConst<C>>(fptr, &instance);
	}

	template<typename L>
	requires Concepts::Function<L, ReturnType, Args...>
		Function(const L& closure)
	{
		_cc = new ClosureContainer<L>(closure);
	}

	Function(const Function& other)
	{
		if (other._cc)
		{
			_cc = (other._cc == (void*)other._static) ? other._cc->Clone(_static) : other._cc->Clone();
			_fptr = nullptr;
		}
		else
		{
			_cc = nullptr;
			_fptr = other._fptr;
		}
	}

	Function(Function&& other) noexcept : _fptr(other._fptr), _cc((other._cc == (void*)other._static) ? other._cc->Clone(_static) : other._cc)
	{
		other._fptr = nullptr;
		other._cc = nullptr;
	}

	~Function()
	{
		_DeleteCC();
	}

	Function& operator=(const Function<ReturnType, Args...>& other)
	{
		_DeleteCC();

		if (other._cc)
		{
			_cc = (other._cc == (void*)other._static) ? other._cc->Clone(_static) : other._cc->Clone();
			_fptr = nullptr;
		}
		else
		{
			_cc = nullptr;
			_fptr = other._fptr;
		}

		return *this;
	}

	Function& operator=(Function<ReturnType, Args...>&& other) noexcept
	{
		_DeleteCC();

		_cc = (other._cc == (void*)other._static) ? other._cc->Clone(_static) : other._cc;
		_fptr = other._fptr;
		other._cc = nullptr;
		other._fptr = nullptr;
		return *this;
	}

	ReturnType operator()(Args... args) const
	{
		if (_cc) return (*_cc)(std::forward<Args>(args)...);
		return _fptr(std::forward<Args>(args)...);
	}

	operator bool() const
	{
		return _cc || _fptr;
	}
};

typedef Function<void> Callback;

template<typename T>
using Getter = Function<T>;

template<typename T>
using Setter = Function<void, const T&>;

#include "Array.hpp"
#include "String.hpp"

class Context;
using CommandPtr = Function<void, const Array<String>&, const Context&>;

template<typename T>
using ContextualGetter = Function<T, const Context&>;

template<typename T>
using ContextualSetter = Function<void, const T&, const Context&>;
