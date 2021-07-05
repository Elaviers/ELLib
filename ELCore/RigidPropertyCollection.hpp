#pragma once
#include "RigidProperty.hpp"
#include "Buffer.hpp"

class RigidPropertyCollection
{
	Buffer<RigidProperty*> _properties;

public:
	RigidPropertyCollection() {}
	RigidPropertyCollection(const RigidPropertyCollection&) = delete;
	RigidPropertyCollection(RigidPropertyCollection&& other) noexcept : _properties(std::move(other._properties)) {}

	~RigidPropertyCollection();

	RigidPropertyCollection& operator=(const RigidPropertyCollection&) = delete;
	RigidPropertyCollection& operator=(RigidPropertyCollection&& other) noexcept { Clear(); _properties = std::move(other._properties); return *this; }

	const Buffer<RigidProperty*>& GetAll() const { return _properties; }

	void Clear();

	String HandleCommand(const Array<String>& tokens, const Context& ctx) const;
	String HandleCommand(const String& command, const Context& ctx) const { return HandleCommand(command.Split<VAllocator<String>>(" "), ctx); }

	RigidProperty* Find(const String& name) const;
	
	template <typename T>
	RigidVariableProperty<T>* FindVar(const String &name)
	{
		return dynamic_cast<RigidVariableProperty<T>*>(Find(name));
	}

	template <typename T>
	bool Get(const String& name, T& out) const
	{
		RigidVariableProperty<T>* property = FindVar<T>(name);
		if (property) out = property->Get();

		return property != nullptr;
	}

	template <typename T>
	void Set(const String& name, const T& value)
	{
		RigidVariableProperty<T>* property = FindVar<T>(name);
		if (property) property->Set(value);
	}

	//////////

	template <typename T>
	void CreateVar(const String& name, const T& value, byte flags = 0)
	{
		_properties.Add(new RigidValueProperty<T>(name, value, flags));
	}

	template <typename T>
	void Add(const String &name, T& value, byte flags = 0)
	{
		_properties.Add(new RigidReferenceProperty<T>(name, value, flags));
	}

	template <typename T, typename V> void Add(const String& name, const MemberGetter<T, V>& getter, const MemberSetter<T, V>& setter, byte flags = 0) { _properties.Add(new RigidFptrProperty<T, V>(name, getter, setter, flags)); }
	template <typename T, typename V> void Add(const String& name, const MemberGetter<T, V>& getter, const ContextualMemberSetter<T, V>& setter, byte flags = 0) { _properties.Add(new RigidFptrProperty<T, V>(name, getter, setter, flags)); }
	template <typename T, typename V> void Add(const String& name, const ContextualMemberGetter<T, V>& getter, const MemberSetter<T, V>& setter, byte flags = 0) { _properties.Add(new RigidFptrProperty<T, V>(name, getter, setter, flags)); }
	template <typename T, typename V> void Add(const String& name, const ContextualMemberGetter<T, V>& getter, const ContextualMemberSetter<T, V>& setter, byte flags = 0) { _properties.Add(new RigidFptrProperty<T, V>(name, getter, setter, flags)); }
	template <typename T, typename V> void Add(const String& name, const MemberGetter<T, const V&>& getter, const MemberSetter<T, V>& setter, byte flags = 0) { _properties.Add(new RigidFptrProperty<T, V>(name, getter, setter, flags)); }
	template <typename T, typename V> void Add(const String& name, const MemberGetter<T, const V&>& getter, const ContextualMemberSetter<T, V>& setter, byte flags = 0) { _properties.Add(new RigidFptrProperty<T, V>(name, getter, setter, flags)); }
	template <typename T, typename V> void Add(const String& name, const ContextualMemberGetter<T, const V&>& getter, const MemberSetter<T, V>& setter, byte flags = 0) { _properties.Add(new RigidFptrProperty<T, V>(name, getter, setter, flags)); }
	template <typename T, typename V> void Add(const String& name, const ContextualMemberGetter<T, const V&>& getter, const ContextualMemberSetter<T, V>& setter, byte flags = 0) { _properties.Add(new RigidFptrProperty<T, V>(name, getter, setter, flags)); }
};
