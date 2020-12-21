#pragma once
#include "AssetManager.hpp"
#include <ELCore/TextProvider.hpp>

class TextManager : public AssetManager<TextProvider>
{
	virtual TextProvider* _CreateResource(const Buffer<byte>& data, const String& name, const String& extension, const Context& ctx) override;
	virtual void _ResourceRead(TextProvider&, const Buffer<byte>&, const String&, const Context&) override;

public:
	TextManager() : AssetManager<TextProvider>() {}
	virtual ~TextManager() {}
};
