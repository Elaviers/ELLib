#pragma once
#include "Hasher.hpp"
#include "Types.hpp"

class SHA256 : public Hasher<byte[32]>
{
public:
	void Hash(const void* in, size_t length, byte (&out)[32]) const;

};
