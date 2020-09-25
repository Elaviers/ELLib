#pragma once
#include <ELCore/Buffer.hpp>
#include <ELGraphics/Colour.hpp>
#include "GL.hpp"

class GLFramebuffer
{
	GLuint _id;

	union _Data
	{
		struct
		{
			GLuint tColour = 0;
			GLuint rbDepth = 0;

			uint32 w = 0;
			uint32 h = 0;
		} colourDepth;

		_Data() : colourDepth() {}
	} _data;

	enum class Mode { NONE = 0, COLOURDEPTH } _mode;

	Buffer<byte> _texBuffer; //Used for sampling textures

public:
	GLFramebuffer() : _id(0), _mode(Mode::NONE) {}
	GLFramebuffer(const GLFramebuffer&) = delete;
	GLFramebuffer(GLFramebuffer&& other) noexcept : _id(other._id), _data(other._data), _mode(other._mode), _texBuffer(other._texBuffer) {}
	~GLFramebuffer() {}

	void CreateColourDepth(uint32 w, uint32 h);
	void Bind() const;
	void Delete();

	GLuint GetColourTexGL() const { return _mode == Mode::COLOURDEPTH ? _data.colourDepth.tColour : 0; }

	Colour SampleColour(uint32 x, uint32 y);

	static void Unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
};
