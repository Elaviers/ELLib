#pragma once
#include "Material.hpp"
#include "Texture.hpp"
#include <ELCore/Buffer.hpp>
#include <ELCore/SharedPointer.hpp>
#include <ELMaths/UVRect.hpp>

class MaterialGrid : public Material
{
	SharedPointer<const Texture> _texture;

	Buffer<int> _rowHeights;
	Buffer<int> _columnWidths;

	Buffer<UVRect> _elements;

	void _TryCalculateElements();

	void _CMD_texture(const Buffer<String> &args, const Context&);
	void _CMD_rows(const Buffer<String> &args, const Context&);
	void _CMD_columns(const Buffer<String> &args, const Context&);

public:
	MaterialGrid() : Material(ERenderChannels::UNLIT), _texture(nullptr) {}

	virtual ~MaterialGrid() {}

	const SharedPointer<const Texture>& GetTexture() const { return _texture; }
	void SetDiffuse(const SharedPointer<const Texture>& texture) { _texture = texture; }

	virtual const PropertyCollection& GetProperties() override;

	virtual void Apply(RenderEntry&, const MaterialParam *param) const override;

	const UVRect& GetElement(int r, int c) const { return _elements[r * _columnWidths.GetSize() + c]; }
};
