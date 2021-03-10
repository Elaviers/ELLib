#include "Font_TTF.hpp"
#include "Colour.hpp"
#include "FontManager.hpp"
#include "RenderCommand.hpp"
#include "RenderQueue.hpp"
#include <ELCore/Context.hpp>
#include <ELCore/MacroUtilities.hpp>
#include <ELMaths/Transform.hpp>

const PropertyCollection& FontTTF::GetProperties()
{
	static PropertyCollection properties;

	DO_ONCE_BEGIN;
	properties.AddCommand("filename", MemberCommandPtr<FontTTF>(&FontTTF::_CMD_LoadFont));
	properties.Add<int>("rendersize", offsetof(FontTTF, _size));
	DO_ONCE_END;

	return properties;
}

void FontTTF::_CMD_LoadFont(const Buffer<String>& args, const Context& ctx)
{
	if (args.GetSize() > 0)
	{
		FontManager* fontManager = ctx.GetPtr<FontManager>();
		FT_Library* ftLib = ctx.GetPtr<FT_Library>();
		if (fontManager && ftLib)
		{
			String fp = args[0];

			const Buffer<String> paths = fontManager->GetPaths();
			for (size_t i = 0; i < paths.GetSize(); ++i)
			{
				fp = paths[i] + args[0];

				if (IO::FileExists(fp.GetData()))
					break;
			}

			FT_Error error = FT_New_Face(*ftLib, fp.GetData(), 0, &_face);
			if (error)
			{
				Debug::Error(CSTR("Could not load font file \"", fp, '\"'));
				return;
			}

			error = FT_Set_Pixel_Sizes(_face, 0, _size);
			if (error)
				Debug::Error("FT_Set_Pixel_Sizes error");

			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

			for (int i = 0; i < 128; ++i)
			{
				error = FT_Load_Char(_face, i, FT_LOAD_RENDER);

				if (error)
				{
					Debug::PrintLine(CSTR("COULD NOT LOAD CHAR \'", (char)i, '\''));
					continue;
				}

				GLuint texture;
				glGenTextures(1, &texture);
				glBindTexture(GL_TEXTURE_2D, texture);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
					_face->glyph->bitmap.width, _face->glyph->bitmap.rows, 0,
					GL_RED, GL_UNSIGNED_BYTE, _face->glyph->bitmap.buffer);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				TTFGlyph glyph = {
					texture,
					Vector2((float)_face->glyph->bitmap.width, (float)_face->glyph->bitmap.rows),
					Vector2((float)_face->glyph->bitmap_left, (float)_face->glyph->bitmap_top),
					_face->glyph->advance.x
				};

				_charMap[(char)i] = glyph;
			}

			_descender = _face->descender >> 6;

			FT_Done_Face(_face);
		}
	}
}

float FontTTF::CalculateStringWidth(const char* string, float scaleX, size_t maxChars) const
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
			const TTFGlyph* space = _charMap.Get(' ');
			float stopWidth = (space->advance >> 6) * scale * 5.f;
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
			const TTFGlyph* glyph = _charMap.Get(*c);
			if (glyph)
				currentLineW += (float)(glyph->advance >> 6) * scale;
		}

		if (++i == maxChars) break;
	}

	return Maths::Max(longestWidth, currentLineW);
}

size_t FontTTF::GetPositionOf(float pX, float pY, const char* string, const Transform& transform, float lineHeight, bool round) const
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
			const TTFGlyph* space = _charMap.Get(' ');
			float stopWidth = (space->advance >> 6) * scale * 5.f;
			float nextStop = Maths::Trunc(currentLineW, stopWidth) + stopWidth;
			currentCharW = (nextStop - currentLineW);
		}
		else
		{
			const TTFGlyph* glyph = _charMap.Get(*c);
			if (glyph)
			{
				currentCharW = (glyph->advance >> 6) * scale;
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

	return StringLength(string);
}

void FontTTF::RenderString(RenderEntry& e, const char* string, const Transform& transform, float lineHeight, const RectF& clip) const
{
	const Vector3& rotation = transform.GetRotation().GetEuler();
	const Vector3 advanceDirection = transform.GetRightVector();
	const Vector3 downDirection = -1.f * transform.GetUpVector();
	
	const float scale = transform.GetScale().x / (float)_size;
	const float yOffset = (float)_descender * scale;

	const bool clipX = clip.w > 0;
	const bool clipY = clip.h > 0;
	float xClipStart = clip.x;
	float yClipStart = clip.y;
	float xClipEnd = xClipStart + clip.w;
	float yClipEnd = yClipStart + clip.h;

	if (xClipStart > xClipEnd) Utilities::Swap(xClipStart, xClipEnd);
	if (yClipStart > yClipEnd) Utilities::Swap(yClipStart, yClipEnd);

	bool clipped = false;

	float line = 0.f;
	float x = 0.f;

	float gX = 0.f, gY = 0.f, gW, gH;
	
	Vector3 rootPos = transform.GetPosition() + downDirection * yOffset;

	e.AddSetIsFont(true);

	for (const char* c = string; *c != '\0'; ++c)
	{
		if (*c == 0x01)
		{
			e.AddSetColour(Colour::FromColourCode(c));

			c += 4;
		}
		else if (*c == '\n' && lineHeight)
		{
			++line;
			x = 0.f;
			
			rootPos = transform.GetPosition() + downDirection * (yOffset + lineHeight * line);
		}
		else if (*c == '\t')
		{
			const TTFGlyph* space = _charMap.Get(' ');
			float stopWidth = (space->advance >> 6) * scale * 5.f;
			x = Maths::Trunc(x, stopWidth) + stopWidth;
		}
		else if (!clipped)
		{
			const TTFGlyph* glyph = _charMap.Get(*c);
			if (glyph)
			{
				gW = glyph->size.x * scale;
				gH = glyph->size.y * scale;

				gX = glyph->bearing.x * scale;
				gY = glyph->bearing.y * scale;
				float gStart = x + gX;
				float gEnd = gStart + gW;

				Vector2 uvOffset = Vector2(0.f, 0.f);
				Vector2 uvScale = Vector2(1.f, 1.f);
				
				if (!clipX || gEnd > xClipStart)
				{
					if (clipX)
					{
						if (gStart < xClipStart)
						{
							//gEnd is past clipping point

							float gWClipped = gEnd - xClipStart;
							gX += xClipStart - gStart;
							uvScale.x = gWClipped / gW;
							uvOffset.x = 1.f - uvScale.x;
							gW = gWClipped;
						}
						else if (gEnd >= xClipEnd)
						{
							clipped = true;

							if (gStart >= xClipEnd)
								gW = 0;
							else
							{
								float gWClipped = xClipEnd - gStart;
								uvScale.x *= gWClipped / gW;
								gW = gWClipped;
							}
						}
					}

					if (clipY)
					{
						float gTop = gY - lineHeight * line - yOffset;
						float gBottom = gTop - gH;

						if (gBottom < yClipStart)
						{
							float gHClipped = gTop - yClipStart;
							uvScale.y = gHClipped / gH;
							gH = gHClipped;
						}
						else if (gTop > yClipEnd)
						{
							if (gBottom >= yClipEnd)
								gH = 0;
							else
							{
								float gHClipped = yClipEnd - gBottom;
								gY += yClipEnd - gTop;
								uvScale.y = gHClipped / gH;
								uvOffset.y = 1.f - uvScale.y;
								gH = gHClipped;
							}
						}
					}

					if (gW && gH)
					{
						e.AddSetTransform(Matrix4::Transformation(
							rootPos +
							(advanceDirection * (gStart + gX + gW / 2.f)) +
							(downDirection * (gH / 2.f - gY)),
							rotation,
							Vector3(gW, gH, 1.f)
						));

						e.AddSetUVOffset(uvOffset);
						e.AddSetUVScale(uvScale);

						e.AddSetTextureGL(glyph->texID, 0);
						e.AddCommand(RCMDRenderMesh::PLANE);
					}
				}

				x += (float)(glyph->advance >> 6) * scale;
			}
		}
	}

	e.AddSetIsFont(false);
}
