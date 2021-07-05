#include "TextProvider.hpp"
#include "Buffer.hpp"
#include "IteratorUtils.hpp"

Text TextProvider::Get(const String& key) const
{
	const String* value = _strings.TryGet(key);
	return value ? Text(value) : Text(String::Concat('[', key, ']'));
}

void TextProvider::Set(const String& key, const String& value)
{
	_strings[key] = value;
}

void TextProvider::Set(const String& data)
{
	Buffer<String> lines = data.Split("\r\n");
	for (const String& line : lines)
	{
		auto sep = IteratorUtils::IndexOf(line.begin(), line.end(), '=');
		if (sep > 0 && sep != IteratorUtils::INVALID_INDEX)
		{
			Set(line.SubString(0, sep), line.SubString((size_t)sep + 1));
		}
	}
}