#pragma once
#include <cstdint>
#include "Hasher.hpp"

class MurmurHash3_32 : public Hasher<uint32_t>
{
	uint32_t _seed;

public:
	constexpr MurmurHash3_32(uint32_t seed) noexcept : _seed(seed) {}

	void Hash(const void* in, size_t length, uint32_t& out) const;
};

class MurmurHash3_32_FixedSeed : public Hasher<uint32_t>
{
public:
	void Hash(const void* in, size_t length, uint32_t& out) const;
};
