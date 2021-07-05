#include "MurmurHash3.hpp"
#include <cstdint>

__forceinline constexpr uint32_t rotl32(uint32_t x, int8_t r) noexcept
{
	return (x << r) | (x >> (32 - r));
}

__forceinline constexpr uint32_t getblock32(const uint32_t* p, int i) noexcept
{
	return p[i];
}

__forceinline constexpr uint32_t fmix32(uint32_t h) noexcept
{
	h ^= h >> 16;
	h *= 0x85ebca6b;
	h ^= h >> 13;
	h *= 0xc2b2ae35;
	h ^= h >> 16;

	return h;
}

constexpr uint32_t MurmurHash3_x86_32(const void* key, int len, uint32_t seed)
{
	const uint8_t* data = (const uint8_t*)key;
	const int nblocks = len / 4;

	uint32_t h1 = seed;

	const uint32_t c1 = 0xcc9e2d51;
	const uint32_t c2 = 0x1b873593;

	//----------
	// body

	const uint32_t* blocks = (const uint32_t*)(data + nblocks * 4);

	for (int i = -nblocks; i; i++)
	{
		uint32_t k1 = getblock32(blocks, i);

		k1 *= c1;
		k1 = rotl32(k1, 15);
		k1 *= c2;

		h1 ^= k1;
		h1 = rotl32(h1, 13);
		h1 = h1 * 5 + 0xe6546b64;
	}

	//----------
	// tail

	const uint8_t* tail = (const uint8_t*)(data + nblocks * 4);

	uint32_t k1 = 0;

	switch (len & 3)
	{
	case 3: k1 ^= tail[2] << 16;
	case 2: k1 ^= tail[1] << 8;
	case 1: k1 ^= tail[0];
		k1 *= c1; k1 = rotl32(k1, 15); k1 *= c2; h1 ^= k1;
	};

	//----------
	// finalization

	h1 ^= len;

	h1 = fmix32(h1);
	return h1;
}

void MurmurHash3_32::Hash(const void* in, size_t len, uint32_t& out) const
{
	out = MurmurHash3_x86_32(in, len, _seed);
}

void MurmurHash3_32_FixedSeed::Hash(const void* in, size_t len, uint32_t& out) const
{
	constexpr const uint32_t fixedSeed = 0x6942faca;
	out = MurmurHash3_x86_32(in, len, fixedSeed);
}