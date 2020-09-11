#include "FontManager.hpp"
#include "Font.hpp"
#include "Font_Texture.hpp"
#include "Font_TTF.hpp"

Font* FontManager::_CreateResource(const Buffer<byte>& data, const String& name, const String& extension, const Context& ctx)
{
	Font* font = Font::FromText(data, ctx);

	if (font == nullptr)
		Debug::Error(CSTR("Unknown type for font \"", name, '\"'));

	return font;
}
