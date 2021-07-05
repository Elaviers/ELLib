#include "FontManager.hpp"
#include "Font.hpp"
#include "Font_Texture.hpp"
#include "Font_TTF.hpp"

Font* FontManager::_CreateResource(const Array<byte>& data, const String& name, const String& extension, const Context& ctx)
{
	Font* font = Font::FromText(String(data.begin(), data.GetSize()), ctx);

	if (font == nullptr)
		Debug::Error(CSTR("Unknown type for font \"", name, '\"'));

	return font;
}
