#pragma once
#include "Font.hpp"
#include "Glyph.hpp"
#include "Texture.hpp"
#include <ELCore/Function.hpp>
#include <ELCore/Hashmap.hpp>
#include <ELCore/SharedPointer.hpp>
#include <ELCore/String.hpp>
#include <ELMaths/Transform.hpp>

class FontTexture : public Font
{
	SharedPointer<const Texture> _texture;
	Hashmap<byte, Glyph> _charMap;

	int _size = 0;
	int _rowH = 0;
	int _yOffset = 0;

	void _CMD_texture(const Array<String>& args, const Context& ctx);
	void _CMD_region(const Array<String>& args, const Context& ctx);

protected:
	virtual void ReadText(const String&, const Context&) override;

public:
	FontTexture() {}

	virtual ~FontTexture() {}

	virtual const PropertyCollection& GetProperties() override;

	virtual float CalculateStringWidth(const char* string, float scaleX, size_t maxChars = 0) const override;
	virtual size_t GetPositionOf(float x, float y, const char* string, const Transform& transform, float lineHeight = 0.f, bool round = true) const override;

	virtual void RenderString(RenderEntry&, const char* string, const Transform & transform, float lineHeight = 0.f, const RectF& clip = RectF()) const override;

	void RenderString(RenderEntry& e, const char* string, const Transform& transform) const { return RenderString(e, string, transform, -transform.GetScale().y); }

	void BindTexture() const { if (_texture) _texture->Bind(0); }
};
