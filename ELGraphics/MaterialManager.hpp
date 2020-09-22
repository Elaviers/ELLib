#pragma once
#include <ELSys/AssetManager.hpp>
#include "Material.hpp"
#include "TextureManager.hpp"
#include <ELCore/FunctionPointer.hpp>

class MaterialManager : public AssetManager<Material>
{
private:
	virtual Material* _CreateResource(const Buffer<byte>& data, const String& name, const String& extension, const Context&) override;
	virtual bool _CreateAlternative(Material*& material, const String& name, const Context&) override;

public:
	MaterialManager() : AssetManager() {}
	virtual ~MaterialManager() {}

	void Initialise(const TextureManager&);
};
 