#pragma once
#include "AssetManager.hpp"
#include <ELCore/TextProvider.hpp>

class TextManager : public AssetManager<TextProvider>
{
	virtual TextProvider* _CreateResource(const Array<byte>& data, const String& name, const String& extension, const Context& ctx) override;
	virtual void _ResourceRead(TextProvider&, const Array<byte>&, const String&, const Context&) override;

public:
	TextManager() : AssetManager<TextProvider>() {}
	virtual ~TextManager() {}
};
