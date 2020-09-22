#include "TextureManager.hpp"
#include "IO.hpp"
#include "MaterialManager.hpp"
#include <ELSys/Debug.hpp>
#include <ELSys/GL.hpp>
#include <vector>

Texture* TextureManager::_CreateResource(const Buffer<byte>& data, const String& name, const String& extension, const Context& ctx)
{
	if (extension == ".png")
	{
		Texture* tex = IO::ReadPNGFile(data, _maxMipLevels, _maxAnisotropy);

		if (tex == nullptr)
			Debug::Error(CSTR("Could not load texture \"", name, '\"'));

		return tex;
	}

	Texture* tex = Asset::FromText<Texture>(data, ctx);

	if (tex == nullptr)
		Debug::Error(CSTR("Could not load texture \"", name, '\"'));

	return tex;
}

void TextureManager::_ResourceRead(Texture& texture, const Buffer<byte>& data, const String& extension, const Context&)
{
	if (extension == ".png")
	{
		Buffer<byte> pngData;
		unsigned int w, h;
		if (IO::ReadPNGFile(data, pngData, w, h))
		{
			texture.Change(pngData.Data(), w, h, _maxMipLevels, _maxAnisotropy);
		}
	}
}

void TextureManager::Initialise()
{
	byte dataBlack[4] = { 0, 0, 0, 255 };
	byte dataWhite[4] = { 255, 255, 255, 255 };
	byte dataGrey[4] = { 127, 127, 127, 255 };
	byte dataNormalDefault[4] = { 127, 127, 255, 255 };
	
	Texture::Info colourInfo;
	colourInfo.aniso = 1;
	colourInfo.mipLevels = 1;
	colourInfo.minFilter = colourInfo.magFilter = GL_NEAREST;

	_colours.black = new Texture();
	_colours.white = new Texture();
	_colours.grey = new Texture();
	_colours.normalDefault = new Texture();

	_colours.black->info = colourInfo;
	_colours.white->info = colourInfo;
	_colours.grey->info = colourInfo;
	_colours.normalDefault->info = colourInfo;

	_colours.black->Change(dataBlack, 1, 1);
	_colours.white->Change(dataWhite, 1, 1);
	_colours.grey->Change(dataGrey, 1, 1);
	_colours.normalDefault->Change(dataNormalDefault, 1, 1);

	SharedPointerData<Texture>& dBlack = _MapValue("black"), & dWhite = _MapValue("white"), & dGrey = _MapValue("grey"), & dNormalDefault = _MapValue("unitnormal");

	dBlack.SetPtr(_colours.black);
	dWhite.SetPtr(_colours.white);
	dGrey.SetPtr(_colours.grey);
	dNormalDefault.SetPtr(_colours.normalDefault);

	_colours.tBlack = SharedPointer<Texture>(dBlack);
	_colours.tWhite = SharedPointer<Texture>(dWhite);
	_colours.tGrey = SharedPointer<Texture>(dGrey);
	_colours.tNormalDefault = SharedPointer<Texture>(dNormalDefault);
}
