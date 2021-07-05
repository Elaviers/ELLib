#pragma once
#include "Map.hpp"
#include <ELSys/Debug.hpp>
#include <typeinfo> //typeid

class Context
{
private:
	Map<size_t, void*> _ptrs;

public:
	Context() {}
	~Context() {}

	static const Context& Empty() { static Context ctx; return ctx; }

	template <typename T>
	void Set(T* ptr)
	{
		_ptrs[typeid(T).hash_code()] = ptr;
	}

	template <typename T, typename ...ARGS>
	void Set(T* ptr, ARGS ...more)
	{
		Set(ptr);
		Set(more...);
	}

	template <typename T>
	T* GetPtr(bool assert = true) const
	{
		void* const * ptr = _ptrs.TryGet(typeid(T).hash_code());

		Debug::Assert(!assert || ptr != nullptr, CSTR("Context::GetPtr could not find pointer for type ", typeid(T).name()));

		return ptr ? (T*)*ptr : nullptr;
	}
};
