#include "NetUtils.hpp"

uint16 NetUtils::ReadNetMultiByte_uint15(ByteReader& reader)
{
	byte byte1 = reader.Read_byte();

	if (byte1 & 0x80)
	{
		byte byte2 = reader.Read_byte();
		return byte2 | ((byte1 & 0x7F) << 8);
	}

	return byte1;
}

void NetUtils::WriteNetMultiByte_uint15(ByteWriter& writer, uint16 value)
{
	if (value > 0x007F)
	{
		writer.Write_byte(0x80 | (value & 0x7F00));
	}

	writer.Write_byte(value & 0x00FF);
}
