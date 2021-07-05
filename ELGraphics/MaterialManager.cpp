#include "MaterialManager.hpp"
#include "IO.hpp"
#include "Material_Grid.hpp"
#include "Material_Surface.hpp"
#include "Material_Sprite.hpp"
#include "TextureManager.hpp"
#include <ELCore/Utilities.hpp>
#include <ELSys/Debug.hpp>

Material* MaterialManager::_CreateResource(const Array<byte>& data, const String& name, const String& ext, const Context& ctx)
{
	Material* mat = Material::FromText(String(data.begin(), data.GetSize()), ctx);

	if (mat == nullptr)
		Debug::Error(CSTR("Could not load material \"", name, '\"'));

	return mat;
}

bool MaterialManager::_CreateAlternative(Material*& material, const String& name, const Context& ctx)
{
	if (material)
	{
		//The material has been created by an _all file
		//Look in the texturemanager for a texture with the same name
		
		SharedPointer<const Texture> texture = ctx.GetPtr<TextureManager>()->Get(name, ctx);
		if (texture && material->SetDefaultTexture(texture))
			return true;
	}

	return false; //Can't do anything now
}

Buffer<const AssetManagerBase*> MaterialManager::_GetFallbackManagers(const Context& ctx) const
{
	return { (const AssetManagerBase*)ctx.GetPtr<TextureManager>() };
}

void MaterialManager::Initialise(const TextureManager& tm)
{
	_MapValue("white").SetPtr(new MaterialSurface(tm.White(), tm.UnitNormal(), tm.White()));
}
