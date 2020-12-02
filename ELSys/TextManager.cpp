#include "TextManager.hpp"

TextProvider* TextManager::_CreateResource(const Buffer<byte>& data, const String& name, const String& extension, const Context& ctx)
{
	TextProvider* provider = new TextProvider();
	provider->Set(data);
	return provider;
}

void TextManager::_ResourceRead(TextProvider& provider, const Buffer<byte>& data, const String& extension, const Context& ctx)
{
	provider.Set(data);
}
