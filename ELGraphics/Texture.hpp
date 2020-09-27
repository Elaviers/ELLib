#pragma once
#include <ELCore/Asset.hpp>
#include <ELCore/Buffer.hpp>
#include <ELMaths/Maths.hpp>
#include <ELSys/GLTexture.hpp>

class Texture : public Asset
{
public:
	struct Info
	{
		byte aniso;
		byte mipLevels;
		GLint minFilter;
		GLint magFilter;
		GLint wrap;

		void CMD_min(const Buffer<String>& args, const Context&);
		void CMD_mag(const Buffer<String>& args, const Context&);
		void CMD_wrap(const Buffer<String>& args, const Context&);

		void SetDefault()
		{
			aniso = 16;
			mipLevels = 8;
			minFilter = GL_LINEAR_MIPMAP_LINEAR;
			magFilter = GL_NEAREST;
			wrap = GL_REPEAT;
		}

		Info()
		{
			SetDefault();
		}
	};


private:
	uint32 _width;
	uint32 _height;

	GLTexture _glTexture;

	void _CMD_img(const Buffer<String>& args, const Context&);

public:
	Info info;

	Texture() : _width(0), _height(0) {}
	virtual ~Texture() { _glTexture.Delete(); }

	const GLTexture& GL() const { return _glTexture; }

	virtual const PropertyCollection& GetProperties() override;

	bool IsValid() const { return _glTexture.IsValid(); }

	uint32 GetWidth() const { return _width; }
	uint32 GetHeight() const { return _height; }

	void Change(const byte* data, uint32 w, uint32 h, byte maxMipLevels = 0, byte maxAnisotropy = 0)
	{
		_width = w;
		_height = h;

		_glTexture.Delete();
		_glTexture.Create(_width, _height, data,
			maxMipLevels ? Maths::Min(maxMipLevels, info.mipLevels) : info.mipLevels,
			maxAnisotropy ? Maths::Min(maxAnisotropy, info.aniso) : info.aniso,
			info.minFilter,
			info.magFilter,
			info.wrap);
	}

	void Bind(int unit) const
	{
		_glTexture.Bind(unit);
	}
};
