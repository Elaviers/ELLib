#pragma once
#include <ElCore/ByteReader.hpp>
#include <ElCore/ByteWriter.hpp>

namespace NetUtils
{
	//Reads 1 byte if value is <= 127, otherwise reads 2
	uint16 ReadNetMultiByte_uint15(ByteReader& reader);

	//Writes 1 byte if value is <= 127, otherwise writes 2
	void WriteNetMultiByte_uint15(ByteWriter& writer, uint16 value);

};
