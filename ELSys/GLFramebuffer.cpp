#include "GLFramebuffer.hpp"
#include <ELSys/Debug.hpp>

void _SetupTextures(GLenum internalColourFormat, GLenum colourFormat, GLuint colourTex, GLuint depthRB, uint32 w, uint32 h)
{
	glBindTexture(GL_TEXTURE_2D, colourTex);
	glTexImage2D(GL_TEXTURE_2D, 0, internalColourFormat, w, h, 0, colourFormat, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glBindRenderbuffer(GL_RENDERBUFFER, depthRB);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);
}

void GLFramebuffer::_ChangeConfiguration(Mode newMode, uint32 w, uint32 h)
{
	if (newMode == GLFramebuffer::Mode::NONE)
	{
		Delete();
		return;
	}

	GLenum internalColourFormat = 0;
	GLenum colourFormat = 0;

	if (newMode == GLFramebuffer::Mode::COLOURDEPTH)
	{
		internalColourFormat = colourFormat = GL_RGB;
	}
	else if (newMode == GLFramebuffer::Mode::RG32UI_DEPTH)
	{
		internalColourFormat = GL_RG32UI;
		colourFormat = GL_RG_INTEGER;
	}

	if (_mode != GLFramebuffer::Mode::NONE)
	{
		if (w && h)
		{
			_SetupTextures(internalColourFormat, colourFormat, _data.colourTexture, _data.depthRenderBuffer, w, h);
			GLFramebuffer::Unbind();

			_mode = newMode;
			_data.w = w;
			_data.h = h;
		}
	}
	else if (w && h)
	{
		glGenFramebuffers(1, &_id);
		glBindFramebuffer(GL_FRAMEBUFFER, _id);

		glGenTextures(1, &_data.colourTexture);
		glGenRenderbuffers(1, &_data.depthRenderBuffer);

		_SetupTextures(internalColourFormat, colourFormat, _data.colourTexture, _data.depthRenderBuffer, w, h);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _data.depthRenderBuffer);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _data.colourTexture, 0);

		GLenum buffers[] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, buffers);
		glReadBuffer(GL_COLOR_ATTACHMENT0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			Debug::Message("Could not set up a framebuffer properly..", "OpenGL error");

		GLFramebuffer::Unbind();

		_mode = newMode;
		_data.w = w;
		_data.h = h;
	}
}

void GLFramebuffer::CreateColourDepth(uint32 w, uint32 h)
{
	_ChangeConfiguration(Mode::COLOURDEPTH, w, h);
}

void GLFramebuffer::Create_RG32UI_Depth(uint32 w, uint32 h)
{
	_ChangeConfiguration(Mode::RG32UI_DEPTH, w, h);
}

void GLFramebuffer::Bind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, _id);
}

void GLFramebuffer::Delete()
{
	if (_mode != Mode::NONE)
	{
		glDeleteTextures(1, &_data.colourTexture);
		glDeleteRenderbuffers(1, &_data.depthRenderBuffer);
	}

	glDeleteFramebuffers(1, &_id);
	_mode = Mode::NONE;
}

Colour GLFramebuffer::SampleColour(uint32 x, uint32 y)
{
	if (_mode != Mode::NONE)
	{
		Bind();

		float pixel[3];
		glReadPixels(x, y, 1, 1, GL_RGB, GL_FLOAT, pixel);

		Unbind();
		return Colour(pixel[0], pixel[1], pixel[2]);
	}

	return Colour(0.f, 0.f, 0.f, 0.f);
}

bool GLFramebuffer::SampleIntegers(uint32 x, uint32 y, uint32 data[4])
{
	if (_mode != Mode::NONE)
	{
		Bind();
		glReadPixels(x, y, 1, 1, GL_RG_INTEGER, GL_UNSIGNED_INT, data);
		Unbind();

		return true;
	}

	return false;
}
