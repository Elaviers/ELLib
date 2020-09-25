#pragma once
#include <ELCore/Asset.hpp>
#include "ERenderChannels.hpp"
#include "RenderEntry.hpp"
#include "Texture.hpp"
#include <ELCore/String.hpp>
#include <ELCore/SharedPointer.hpp>

struct MaterialParam;

class Material : public Asset
{
protected:
	ERenderChannels _RenderChannels;

	Material(ERenderChannels channels) : _RenderChannels(channels) { }

public:
	virtual ~Material() {}

	static Material* FromText(const String&, const Context&);

	virtual void Apply(RenderEntry&, const MaterialParam* param = nullptr) const = 0;

	virtual bool SetDefaultTexture(const SharedPointer<const Texture>& texture) { return false; }
	virtual const SharedPointer<const Texture>& GetDefaultTexture() const { static SharedPointer<const Texture> null; return null; }

	ERenderChannels GetRenderChannels() const { return _RenderChannels; }
};
