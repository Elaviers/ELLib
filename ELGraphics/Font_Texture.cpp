#include "Font_Texture.hpp"
#include "Colour.hpp"
#include "RenderCommand.hpp"
#include "RenderEntry.hpp"
#include "TextureManager.hpp"
#include <ELCore/Context.hpp>
#include <ELCore/MacroUtilities.hpp>

const PropertyCollection& FontTexture::GetProperties()
{
	static PropertyCollection properties;

	DO_ONCE_BEGIN;
	properties.AddCommand("texture", MemberCommandPtr<FontTexture>(&FontTexture::_CMD_texture));
	properties.AddCommand("region", MemberCommandPtr<FontTexture>(&FontTexture::_CMD_region));
	properties.Add<int>("size", offsetof(FontTexture, _size));
	properties.Add<int>("row_h", offsetof(FontTexture, _rowH));
	properties.Add<int>("y_offset", offsetof(FontTexture, _yOffset));
	DO_ONCE_END;

	return properties;
}

void FontTexture::_CMD_texture(const Array<String>& args, const Context& ctx)
{
	if (args.GetSize() > 0)
	{
		_texture = ctx.GetPtr<TextureManager>()->Get(args[0], ctx);
	}
}

void FontTexture::_CMD_region(const Array<String>& args, const Context& ctx)
{
	if (args.GetSize() >= 6)
	{
		if (args[0] == "space")
		{
			Glyph& glyph = _charMap[' '];
			glyph.uv.x = (float)args[1].ToInt();
			glyph.uv.y = (float)args[2].ToInt();
			glyph.uvSize.x = (float)args[3].ToInt();
			glyph.uvSize.y = (float)args[4].ToInt();
			glyph.advance = args[5].ToInt();

			glyph.width = (int)(glyph.uvSize.x * _texture->GetWidth());
			if (glyph.width == 0)
				glyph.width = glyph.advance;
		}
	}
}

void FontTexture::ReadText(const String & string, const Context& ctx)
{
	Buffer<String> lines = string.Split("\r\n");

	for (uint32 i = 0; i < lines.GetSize(); ++i)
	{
		String loweredLine = lines[i].ToLower();

		if (loweredLine == "glyphs")
		{
			if (!_texture) return;

			int cX = 0;
			int cY = 0;

			i++;
			for (size_t slot = 0; i + slot < lines.GetSize(); ++slot)
			{
				Buffer<String> tokens = lines[i + slot].Split(" ");

				if (tokens.GetSize() > 1)
				{
					Glyph& glyph = _charMap[tokens[0][0]];
					glyph.width = tokens[1].ToInt();
					glyph.advance = tokens[2].ToInt();

					if (cX + glyph.width >= (int)_texture->GetWidth())
					{
						cX = 0;
						cY += _rowH;
					}

					glyph.uv.x = cX / (float)_texture->GetWidth();
					glyph.uv.y = cY / (float)_texture->GetHeight();
					glyph.uvSize.x = glyph.width / (float)_texture->GetWidth();
					glyph.uvSize.y = _rowH / (float)_texture->GetHeight();

					cX += glyph.width;
				}
			}

			break;
		}

		String unused = GetProperties().HandleCommand(this, loweredLine, ctx);
	}
}

float FontTexture::CalculateStringWidth(const char* string, float scaleX, size_t maxChars) const
{
	float longestWidth = 0.f;
	float currentLineW = 0.f;

	float scale = scaleX / (float)_size;
	size_t i = 0;
	for (const char* c = string; *c != '\0'; ++c)
	{
		if (*c == 0x01)
		{
			c += 4;
		}
		else if (*c == '\t')
		{
			const Glyph* space = _charMap.TryGet(' ');
			float stopWidth = space->width * scale * 5.f;
			float nextStop = Maths::Trunc(currentLineW, stopWidth) + stopWidth;

			currentLineW = nextStop;
		}
		else if (*c == '\n')
		{
			if (currentLineW > longestWidth)
				longestWidth = currentLineW;

			currentLineW = 0.f;
		}
		else
		{
			const Glyph* glyph = _charMap.TryGet(*c);
			if (glyph)
				currentLineW += (glyph->width + glyph->advance) * scale;
		}

		if (++i == maxChars) break;
	}

	return Maths::Max(longestWidth, currentLineW);
}

size_t FontTexture::GetPositionOf(float pX, float pY, const char* string, const Transform& transform, float lineHeight, bool round) const
{
	Vector3 advanceDirection = transform.GetRightVector();
	Vector3 downDirection = -1.f * transform.GetUpVector();

	float x = transform.GetPosition().x;
	float y = transform.GetPosition().y;

	float scale = (transform.GetScale().x / (float)_size);

	float line = 0.f;
	float currentLineW = 0.f;
	float currentCharW = 0.f;

	size_t i = 0;
	for (const char* c = string; *c != '\0'; ++c)
	{
		if (*c == 0x01)
		{
			c += 4;
		}
		else if (*c == '\n' && lineHeight)
		{
			++line;
			currentLineW = 0.f;
			currentCharW = 0.f;

			float x = transform.GetPosition().x;
			float y = transform.GetPosition().y;
			x += downDirection.x * lineHeight * line;
			y += downDirection.y * lineHeight * line;
		}
		else if (*c == '\t')
		{
			const Glyph* space = _charMap.TryGet(' ');
			float stopWidth = space->width * scale * 5.f;
			float nextStop = Maths::Trunc(currentLineW, stopWidth) + stopWidth;
			currentCharW = (nextStop - currentLineW);
		}
		else
		{
			const Glyph* glyph = _charMap.TryGet(*c);
			if (glyph)
			{
				currentCharW = (glyph->width + glyph->advance) * scale;
			}
		}

		if (x + advanceDirection.x * currentCharW >= pX)
		{
			if (round && x + advanceDirection.x * currentCharW / 2.f <= pX)
				return i + 1;

			return i;
		}

		x += advanceDirection.x * currentCharW;
		y += advanceDirection.y * currentCharW;
		currentLineW += currentCharW;

		++i;
	}

	return StringUtils::Length(string);
}

void FontTexture::RenderString(RenderEntry& e, const char* string, const Transform& transform, float lineHeight, const RectF& clip) const
{
	const Vector3& rotation = transform.GetRotation().GetEuler();
	const Vector3 advanceDirection = transform.GetRightVector();
	const Vector3 downDirection = -1.f * transform.GetUpVector();

	const float h = transform.GetScale().y;

	const float xScale = transform.GetScale().x / (float)_size;
	const Vector2 halfTexel(.5f / ((float)_texture->GetWidth() * xScale), .5f / ((float)_texture->GetHeight() * xScale));
	const Vector2 texel(1.f / ((float)_texture->GetWidth() * xScale), 1.f / ((float)_texture->GetHeight() * xScale));

	Colour colour = Colour::White;

	float gX = 0.f;
	float gW, gH = h;

	const bool clipX = clip.w > 0;
	const bool clipY = clip.h > 0;
	float xClipStart = clip.x;
	float yClipStart = clip.y;
	float xClipEnd = xClipStart + clip.w;
	float yClipEnd = yClipStart + clip.h;

	if (xClipStart > xClipEnd) Utilities::Swap(xClipStart, xClipEnd);
	if (yClipStart > yClipEnd) Utilities::Swap(yClipStart, yClipEnd);

	float line = 0.f;
	float x = 0.f;
	
	bool flipUVYOffset = false;
	float uvYScale = 1.f;

	float y = 0.f;

	bool clipped = false;
	if (clipY)
	{
		if (yClipEnd < 0|| h <= yClipStart)
			clipped = true;
		else if (yClipStart > 0)
		{
			float gHClipped = h - yClipStart;
			y = yClipStart;
			uvYScale = gHClipped / gH;
			gH = gHClipped;
		}
		else if (yClipEnd < h)
		{
			float gHClipped = yClipEnd;
			uvYScale = gHClipped / gH;
			flipUVYOffset = true;
			gH = gHClipped;
		}
	}

	Vector3 rootPos = transform.GetPosition() - downDirection * (gH / 2.f + y);

	for (const char* c = string; *c != '\0'; ++c)
	{
		if (*c == 0x01)
		{
			//Colour code
			colour = Colour::FromColourCode(c).GetData();
			c += 4;
		}
		else if (*c == '\n' && lineHeight)
		{
			++line;
			x = 0.f;

			y = -lineHeight * line;
			
			gH = h;

			if (clipY)
			{
				if (y >= yClipEnd || y + h <= yClipStart)
					clipped = true;
				else if (y < yClipStart)
				{
					float gHClipped = y + h - yClipStart;
					y += yClipStart - y;
					flipUVYOffset = false;
					uvYScale = gHClipped / gH;
					gH = gHClipped;
				}
				else if (y + h > yClipEnd)
				{
					float gHClipped = yClipEnd - y;
					uvYScale = gHClipped / gH;
					flipUVYOffset = true;
					gH = gHClipped;
				}
				else
				{
					flipUVYOffset = false;
					uvYScale = 1.f;
				}
			}

			rootPos = transform.GetPosition() - downDirection * (gH / 2.f + y);
		}
		else if (*c == '\t')
		{
			const Glyph* space = _charMap.TryGet(' ');
			float stopWidth = space->width * xScale * 5.f;
			x = Maths::Trunc(x, stopWidth) + stopWidth;
		}
		else if (!clipped)
		{
			const Glyph* glyph = _charMap.TryGet(*c);
			if (glyph)
			{
				gX = 0.f;
				gW = glyph->width * xScale;
				float gEnd = x + gW;

				if (!clipX || gEnd > xClipStart)
				{
					Vector2 uvOffset = glyph->uv;
					Vector2 uvScale = glyph->uvSize;

					uvScale.y *= uvYScale;

					if (flipUVYOffset)
						uvOffset.y = glyph->uv.y + glyph->uvSize.y - uvScale.y;

					if (clipX)
					{
						if (x < xClipStart)
						{
							float gWClipped = gEnd - xClipStart;
							gX = xClipStart - x;
							uvScale.x *= gWClipped / gW;
							uvOffset.x = glyph->uv.x + glyph->uvSize.x - uvScale.x;
							gW = gWClipped;
						}
						else if (gEnd >= xClipEnd)
						{
							clipped = true;

							if (x >= xClipEnd)
								gW = 0;
							else
							{
								float gWClipped = xClipEnd - x;
								uvScale.x *= gWClipped / gW;
								gW = gWClipped;
							}
						}
					}

					if (gW != 0)
					{
						e.AddSetTransform(Matrix4::Transformation(
							rootPos + advanceDirection * (x + gX + gW / 2.f),
							rotation,
							Vector3(gW, gH, 1.f)));

						uvOffset += halfTexel;
						uvScale -= texel;
						e.AddSetUVOffset(uvOffset);
						e.AddSetUVScale(uvScale);
						e.AddSetTexture(*_texture, 0);
						e.AddCommand(RCMDRenderMesh::PLANE);
					}
				}
				
				x += (glyph->width + glyph->advance) * xScale;
			}
		}
	}
}

