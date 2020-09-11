#pragma once
#include <ELSys/AssetManager.hpp>
#include "Font.hpp"

class FontManager : public AssetManager<Font>
{
	virtual Font* _CreateResource(const Buffer<byte>& data, const String& name, const String& extension, const Context&) override;

	String _systemPath;

public:
	FontManager() : AssetManager() {}
	virtual ~FontManager() {}

	void SetSystemPath(const String& path) { _systemPath = path; }
};
