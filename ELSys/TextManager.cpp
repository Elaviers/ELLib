#include "TextManager.hpp"

TextProvider* TextManager::_CreateResource(const Array<byte>& data, const String& name, const String& extension, const Context& ctx)
{
	TextProvider* provider = new TextProvider();
	provider->Set(String(data.begin(), data.GetSize()));
	return provider;
}

void TextManager::_ResourceRead(TextProvider& provider, const Array<byte>& data, const String& extension, const Context& ctx)
{
	provider.Set(String(data.begin(), data.GetSize()));
}
