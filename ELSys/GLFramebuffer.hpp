#pragma once
#include <ELCore/Buffer.hpp>
#include <ELGraphics/Colour.hpp>
#include "GL.hpp"

class GLFramebuffer
{
	GLuint _id;

	struct _Data
	{
		GLuint colourTexture = 0;
		GLuint depthRenderBuffer = 0;

		uint32 w = 0;
		uint32 h = 0;
	} _data;

	enum class Mode { NONE = 0, COLOURDEPTH = 1, RG32UI_DEPTH = 2 } _mode;

	void _ChangeConfiguration(Mode newMode, uint32 w, uint32 h);

public:
	GLFramebuffer() : _id(0), _mode(Mode::NONE) {}
	GLFramebuffer(const GLFramebuffer&) = delete;
	GLFramebuffer(GLFramebuffer&& other) noexcept : _id(other._id), _data(other._data), _mode(other._mode) {}
	~GLFramebuffer() {}

	void CreateColourDepth(uint32 w, uint32 h);
	void Create_RG32UI_Depth(uint32 w, uint32 h);
	void Bind() const;
	void Delete();

	GLuint GetColourTexGL() const { return _mode == Mode::COLOURDEPTH ? _data.colourTexture : 0; }

	Colour SampleColour(uint32 x, uint32 y);
	bool SampleIntegers(uint32 x, uint32, uint32 data[4]);

	static void Unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
};
