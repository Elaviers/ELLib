#pragma once
#include "Function.hpp"

template <typename T>
class Handle
{
	T* _obj;
	Function<void, const T&> _destruct;

public:
	Handle() : _obj(nullptr) {}

	Handle(T& ref, const Function<void, const T&>& destruct) : _obj(&ref), _destruct(destruct) {}

	Handle(const Handle&) = delete;
	
	Handle(Handle&& other) : _obj(other._obj), _destruct(std::move(other._destruct))
	{
		other._obj = nullptr;
	}

	~Handle()
	{
		if (_obj) _destruct(*_obj);
	}

	Handle& operator=(const Handle&) = delete;

	Handle& operator=(Handle&& other)
	{
		_obj = other._obj;
		_destruct = std::move(other._destruct);
		return *this;
	}

	bool IsValid() const { return _obj != nullptr; }
	T* operator->() const { return _obj; }
	T& operator*() const { return *_obj; }

	void Release()
	{
		if (_obj)
		{
			_destruct(*_obj);
			_obj = nullptr;
		}
	}
};
