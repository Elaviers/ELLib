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
		GLint wrapx;
		GLint wrapy;

		void CMD_min(const Array<String>& args, const Context&);
		void CMD_mag(const Array<String>& args, const Context&);
		void CMD_wrap(const Array<String>& args, const Context&);

		void SetDefault()
		{
			aniso = 16;
			mipLevels = 8;
			minFilter = GL_LINEAR_MIPMAP_LINEAR;
			magFilter = GL_NEAREST;
			wrapx = wrapy = GL_REPEAT;
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

	void _CMD_img(const Array<String>& args, const Context&);

public:
	Info info;

	Texture() : _width(0), _height(0) {}
	virtual ~Texture() { _glTexture.Delete(); }

	const GLTexture& GL() const { return _glTexture; }

	virtual const PropertyCollection& GetProperties() override;

	bool IsValid() const { return _glTexture.IsValid(); }

	uint32 GetWidth() const { return _width; }
	uint32 GetHeight() const { return _height; }

	void Create(const byte* data, uint32 w, uint32 h, byte maxMipLevels = 0, byte maxAnisotropy = 0)
	{
		_width = w;
		_height = h;

		_glTexture.Delete();
		_glTexture.Create(_width, _height, data,
			maxMipLevels ? Maths::Min(maxMipLevels, info.mipLevels) : info.mipLevels,
			maxAnisotropy ? Maths::Min(maxAnisotropy, info.aniso) : info.aniso,
			info.minFilter, info.magFilter,
			info.wrapx, info.wrapy);
	}

	//Modifies region of mip
	void Modify(int mip, uint32 x, uint32 y, uint32 w, uint32 h, const byte* data)
	{
		_glTexture.Modify(mip, x, y, w, h, data);
	}

	void Bind(int unit) const
	{
		_glTexture.Bind(unit);
	}
};
