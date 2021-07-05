#pragma once
#include "Hashmap.hpp"

template <typename ValueType, typename IdType = uint32, typename AllocatorType = DefaultAllocator<ValueType>>
class NumberedSet
{
public:
	using id_type = IdType;

private:
	using _Value2IdAlloc = Utilities::ReplaceParam<AllocatorType, Pair<ValueType, IdType>>::Type;
	using _Id2ValueAlloc = Utilities::ReplaceParam<AllocatorType, Pair<IdType, ValueType>>::Type;

	Hashmap<ValueType, IdType, MurmurHash3_32_FixedSeed, _Value2IdAlloc> _value2id;
	Hashmap<IdType, ValueType, MurmurHash3_32_FixedSeed, _Id2ValueAlloc> _id2value;

	id_type _nextID;

public:
	constexpr NumberedSet(id_type firstID = (id_type)1, const AllocatorType& allocator = AllocatorType()) noexcept : _value2id(allocator), _id2value(allocator), _nextID(firstID) {}

	constexpr void Set(id_type id, const ValueType& value)
	{
		auto sz = _id2value.GetSize();
		_id2value.Set(id, value);
		if (_id2value.GetSize() > sz)
			_value2id.Set(value, id);
	}

	constexpr id_type Add(const ValueType& value)
	{ 
		id_type id = _value2id.GetOrDefault(value, _nextID);
		if (id == _nextID)
		{
			//added new bucket
			_id2value.Set(id, value);
			++_nextID;
		}

		return id;
	}

	constexpr const ValueType* TryGet(id_type id) const
	{
		return _id2value.TryGet(id);
	}

	constexpr const id_type* IDOf(const ValueType& value)
	{
		return _value2id.TryGet(value);
	}

	constexpr const auto begin() const noexcept { return _id2value.begin(); }
	constexpr const auto end() const noexcept { return _id2value.end(); }
};
