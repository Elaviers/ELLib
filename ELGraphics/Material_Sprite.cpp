#include "Material_Sprite.hpp"
#include "TextureManager.hpp"
#include <ELCore/Context.hpp>
#include <ELCore/MacroUtilities.hpp>

const PropertyCollection& MaterialSprite::GetProperties()
{
	static PropertyCollection properties;

	DO_ONCE(properties.AddCommand("tex", MemberCommandPtr<MaterialSprite>(&MaterialSprite::_CMD_tex)));

	return properties;
}

void MaterialSprite::_CMD_tex(const Buffer<String>& args, const Context& ctx)
{
	if (args.GetSize() > 0)
	{
		TextureManager* textureManager = ctx.GetPtr<TextureManager>();
		if (textureManager)
			_diffuse = textureManager->Get(args[0], ctx);
	}
}

void MaterialSprite::Apply(RenderEntry& e, const MaterialParam* param) const
{
	if (_diffuse) e.AddSetTexture(*_diffuse, 0);
	else e.AddSetTexture(RCMDSetTexture::Type::WHITE, 0);
}
