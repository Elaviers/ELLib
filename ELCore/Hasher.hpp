#pragma once

template <typename HASH_T>
class Hasher
{
public:
	using HashType = HASH_T;
	constexpr void Hash(const void* key, size_t length, HASH_T& out) const { static_assert(false, "Base Hasher::Hash cannot be used"); }
};

namespace Hashing
{
	template <typename HasherType, typename T, typename HASH_T>
	constexpr void HashObject(const HasherType& hasher, const T& object, HASH_T& out)
	{
		hasher.Hash(&object, sizeof(object), out);
	}
}
