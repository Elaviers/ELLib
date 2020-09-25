#include "GLFramebuffer.hpp"
#include <ELSys/Debug.hpp>

void GLFramebuffer::CreateColourDepth(uint32 w, uint32 h)
{
	if (_mode == Mode::COLOURDEPTH)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, _id);
		
		glBindTexture(GL_TEXTURE_2D, _data.colourDepth.tColour);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	
		glBindRenderbuffer(GL_RENDERBUFFER, _data.colourDepth.rbDepth);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);
	}
	else if (_mode == Mode::NONE)
	{
		if (w && h)
		{
			_mode = Mode::COLOURDEPTH;
			glGenFramebuffers(1, &_id);
			glBindFramebuffer(GL_FRAMEBUFFER, _id);

			GLuint colour;
			glGenTextures(1, &colour);
			glBindTexture(GL_TEXTURE_2D, colour);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			_data.colourDepth.tColour = colour;

			GLuint depth;
			glGenRenderbuffers(1, &depth);
			glBindRenderbuffer(GL_RENDERBUFFER, depth);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);
			_data.colourDepth.rbDepth = depth;

			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth);
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, colour, 0);
			
			GLenum buffers[] = { GL_COLOR_ATTACHMENT0 };
			glDrawBuffers(1, buffers);
			glReadBuffer(GL_COLOR_ATTACHMENT0);

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				Debug::Message("Could not set up a framebuffer properly..", "OpenGL error");
		}
		//else Debug::PrintLine("Warning: cannot create a framebuffer with 0 width and/or height");
	}
	else
	{
		Debug::FatalError("A big old framebuffer error that should never happen just happened! Oh no!");
		return;
	}

	_texBuffer.SetSize((size_t)w * h * 3);
	_data.colourDepth.w = w;
	_data.colourDepth.h = h;

	Unbind();
}

void GLFramebuffer::Bind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, _id);
}

void GLFramebuffer::Delete()
{
	if (_mode == Mode::COLOURDEPTH)
	{
		glDeleteTextures(1, &_data.colourDepth.tColour);
		glDeleteRenderbuffers(1, &_data.colourDepth.rbDepth);
	}

	glDeleteFramebuffers(1, &_id);
	_mode = Mode::NONE;
}

Colour GLFramebuffer::SampleColour(uint32 x, uint32 y)
{
	if (_mode == Mode::COLOURDEPTH && _texBuffer.GetSize())
	{
		Bind();

		float pixel[3];
		glReadPixels(x, y, 1, 1, GL_RGB, GL_FLOAT, pixel);

		Unbind();
		return Colour(pixel[0], pixel[1], pixel[2]);
	}

	return Colour(0.f, 0.f, 0.f, 0.f);
}
