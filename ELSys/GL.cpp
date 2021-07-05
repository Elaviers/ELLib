#include "GL.hpp"
#include "Window.hpp"
#include <ELCore/String.hpp>
#include <ELSys/Debug.hpp>

PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
PFNGLGETSTRINGIPROC glGetStringi;

PFNGLBLENDEQUATIONPROC			glBlendEquation;
PFNGLBLENDEQUATIONSEPARATEPROC	glBlendEquationSeparate;
PFNGLBLENDFUNCSEPARATEPROC		glBlendFuncSeparate;
PFNGLBINDSAMPLERPROC			glBindSampler;
PFNGLDRAWELEMENTSBASEVERTEXPROC glDrawElementsBaseVertex;
PFNGLGETATTRIBLOCATIONPROC		glGetAttribLocation;

//Windows
PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;
PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB;
PFNWGLSWAPINTERVALEXTPROC	wglSwapIntervalEXT;

//Vertex Arrays
PFNGLGENVERTEXARRAYSPROC	glGenVertexArrays;
PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
PFNGLBINDVERTEXARRAYPROC	glBindVertexArray;

PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;

//Buffers
PFNGLGENBUFFERSPROC			glGenBuffers;
PFNGLDELETEBUFFERSPROC		glDeleteBuffers;
PFNGLBINDBUFFERPROC			glBindBuffer;
PFNGLBUFFERDATAPROC			glBufferData;

//Frame Buffers
PFNGLGENFRAMEBUFFERSPROC	glGenFramebuffers;
PFNGLBINDFRAMEBUFFERPROC	glBindFramebuffer;
PFNGLFRAMEBUFFERRENDERBUFFERPROC glFramebufferRenderbuffer;
PFNGLFRAMEBUFFERTEXTUREPROC glFramebufferTexture;
PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D;
PFNGLDRAWBUFFERSPROC		glDrawBuffers;
PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus;
PFNGLDELETEFRAMEBUFFERSPROC	glDeleteFramebuffers;

//Programs
PFNGLCREATEPROGRAMPROC		glCreateProgram;
PFNGLDELETEPROGRAMPROC		glDeleteProgram;
PFNGLGETPROGRAMINFOLOGPROC	glGetProgramInfoLog;
PFNGLGETPROGRAMIVPROC		glGetProgramiv;
PFNGLLINKPROGRAMPROC		glLinkProgram;
PFNGLUSEPROGRAMPROC			glUseProgram;

//Shaders
PFNGLCREATESHADERPROC		glCreateShader;
PFNGLDELETESHADERPROC		glDeleteShader;
PFNGLATTACHSHADERPROC		glAttachShader;
PFNGLDETACHSHADERPROC		glDetachShader;
PFNGLGETSHADERINFOLOGPROC	glGetShaderInfoLog;
PFNGLGETSHADERIVPROC		glGetShaderiv;
PFNGLSHADERSOURCEPROC		glShaderSource;
PFNGLCOMPILESHADERPROC		glCompileShader;

//Textures
PFNGLTEXSTORAGE2DPROC		glTexStorage2D;
PFNGLTEXTURESUBIMAGE2DPROC	glTextureSubImage2D;
PFNGLACTIVETEXTUREPROC		glActiveTexture;
PFNGLGENERATEMIPMAPPROC		glGenerateMipmap;

//Render Buffers
PFNGLGENRENDERBUFFERSPROC	glGenRenderbuffers;
PFNGLBINDRENDERBUFFERPROC	glBindRenderbuffer;
PFNGLRENDERBUFFERSTORAGEPROC glRenderbufferStorage;
PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC glRenderbufferStorageMultisample;
PFNGLDELETERENDERBUFFERSPROC glDeleteRenderbuffers;

//Uniform variables
PFNGLGETACTIVEUNIFORMPROC	glGetActiveUniform;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
PFNGLGETUNIFORMFVPROC		glGetUniformfv;
PFNGLGETUNIFORMIVPROC		glGetUniformiv;
PFNGLGETUNIFORMUIVPROC		glGetUniformuiv;
PFNGLGETUNIFORMDVPROC		glGetUniformdv;
PFNGLGETNUNIFORMFVPROC		glGetnUniformfv;
PFNGLGETNUNIFORMIVPROC		glGetnUniformiv;
PFNGLGETNUNIFORMUIVPROC		glGetnUniformuiv;
PFNGLGETNUNIFORMDVPROC		glGetnUniformdv;
PFNGLUNIFORM1FPROC			glUniform1f;
PFNGLUNIFORM2FPROC			glUniform2f;
PFNGLUNIFORM3FPROC			glUniform3f;
PFNGLUNIFORM4FPROC			glUniform4f;
PFNGLUNIFORM1IPROC			glUniform1i;
PFNGLUNIFORM2IPROC			glUniform2i;
PFNGLUNIFORM3IPROC			glUniform3i;
PFNGLUNIFORM4IPROC			glUniform4i;
PFNGLUNIFORM1UIPROC			glUniform1ui;
PFNGLUNIFORM2UIPROC			glUniform2ui;
PFNGLUNIFORM3UIPROC			glUniform3ui;
PFNGLUNIFORM4UIPROC			glUniform4ui;
PFNGLUNIFORM1FVPROC			glUniform1fv;
PFNGLUNIFORM2FVPROC			glUniform2fv;
PFNGLUNIFORM3FVPROC			glUniform3fv;
PFNGLUNIFORM4FVPROC			glUniform4fv;
PFNGLUNIFORM1IVPROC			glUniform1iv;
PFNGLUNIFORM2IVPROC			glUniform2iv;
PFNGLUNIFORM3IVPROC			glUniform3iv;
PFNGLUNIFORM4IVPROC			glUniform4iv;
PFNGLUNIFORM1UIVPROC		glUniform1uiv;
PFNGLUNIFORM2UIVPROC		glUniform2uiv;
PFNGLUNIFORM3UIVPROC		glUniform3uiv;
PFNGLUNIFORM4UIVPROC		glUniform4uiv;
PFNGLUNIFORMMATRIX2FVPROC	glUniformMatrix2fv;
PFNGLUNIFORMMATRIX3FVPROC	glUniformMatrix3fv;
PFNGLUNIFORMMATRIX4FVPROC	glUniformMatrix4fv;
PFNGLUNIFORMMATRIX2X3FVPROC glUniformMatrix2x3fv;
PFNGLUNIFORMMATRIX3X2FVPROC glUniformMatrix3x2fv;
PFNGLUNIFORMMATRIX2X4FVPROC glUniformMatrix2x4fv;
PFNGLUNIFORMMATRIX4X2FVPROC glUniformMatrix4x2fv;
PFNGLUNIFORMMATRIX3X4FVPROC glUniformMatrix3x4fv;
PFNGLUNIFORMMATRIX4X3FVPROC glUniformMatrix4x3fv;

namespace GL
{
	void* Get(const char *functionName)
	{
		void *function = (void*)wglGetProcAddress(functionName);

		if (function == 0 || function == (void*)1 || function == (void*)2 || function == (void*)-1)
		{
			HMODULE module = LoadLibraryA("opengl32.dll");
			if (module)
				function = (void *)GetProcAddress(module, functionName);
		}

		return function;
	}

	void AssertExtension(const char *extension_str, const char *extension)
	{
		if (!StringUtils::Contains(extension_str, extension))
			Debug::FatalError(CSTR("OpenGL extension \"", extension, "\" not supported!"));
	}

	void AssertExtension(const Buffer<const char*>& extensions, const char* extension)
	{
		for (const char* ext : extensions)
			if (StringUtils::Equal(ext, extension))
				return;

		Debug::FatalError(CSTR("OpenGL extension \"", extension, "\" not supported!"));
	}

#define LOAD_GL_FUNC(FUNC, TYPE) FUNC = (TYPE)Get(#FUNC)

	void LoadDummyExtensions()
	{
		LOAD_GL_FUNC(wglCreateContextAttribsARB, PFNWGLCREATECONTEXTATTRIBSARBPROC);
		LOAD_GL_FUNC(wglChoosePixelFormatARB, PFNWGLCHOOSEPIXELFORMATARBPROC);

		if (wglCreateContextAttribsARB == nullptr)
			Debug::FatalError("Could not load vital openGL extension \"wglCreateContextAttribsARB\"!!! Oh no!!!");

		if (wglChoosePixelFormatARB == nullptr)
			Debug::FatalError("Could not load vital openGL extension \"wglChoosePixelFormatARB\"!!! Oh no!!!");
	}

	void LoadExtensions(const Window& window)
	{
		//Windows extensions
		LOAD_GL_FUNC(wglCreateContextAttribsARB, PFNWGLCREATECONTEXTATTRIBSARBPROC);
		LOAD_GL_FUNC(wglChoosePixelFormatARB, PFNWGLCHOOSEPIXELFORMATARBPROC);
		LOAD_GL_FUNC(wglGetExtensionsStringARB, PFNWGLGETEXTENSIONSSTRINGARBPROC);
		LOAD_GL_FUNC(glGetStringi, PFNGLGETSTRINGIPROC);

		int extCount;
		glGetIntegerv(GL_NUM_EXTENSIONS, &extCount);

		Buffer<const char*> extensions;
		extensions.SetSize(extCount);

		for (int i = 0; i < extCount; ++i)
			extensions[i] = (const char*)glGetStringi(GL_EXTENSIONS, i);
		
		const char *winExtensions = wglGetExtensionsStringARB(window._hdc);
		
		AssertExtension(extensions, "WGL_EXT_swap_control");
		AssertExtension(winExtensions, "WGL_EXT_swap_control");

		LOAD_GL_FUNC(wglSwapIntervalEXT, PFNWGLSWAPINTERVALEXTPROC);


		//General
		LOAD_GL_FUNC(glBlendEquation, PFNGLBLENDEQUATIONPROC);
		LOAD_GL_FUNC(glBlendEquationSeparate, PFNGLBLENDEQUATIONSEPARATEPROC);
		LOAD_GL_FUNC(glBlendFuncSeparate, PFNGLBLENDFUNCSEPARATEPROC);
		LOAD_GL_FUNC(glBindSampler, PFNGLBINDSAMPLERPROC);
		LOAD_GL_FUNC(glDrawElementsBaseVertex, PFNGLDRAWELEMENTSBASEVERTEXPROC);
		LOAD_GL_FUNC(glGetAttribLocation, PFNGLGETATTRIBLOCATIONPROC);

		LOAD_GL_FUNC(glGenVertexArrays, PFNGLGENVERTEXARRAYSPROC);
		LOAD_GL_FUNC(glDeleteVertexArrays, PFNGLDELETEVERTEXARRAYSPROC);
		LOAD_GL_FUNC(glBindVertexArray, PFNGLBINDVERTEXARRAYPROC);

		LOAD_GL_FUNC(glVertexAttribPointer, PFNGLVERTEXATTRIBPOINTERPROC);
		LOAD_GL_FUNC(glEnableVertexAttribArray, PFNGLENABLEVERTEXATTRIBARRAYPROC);
		LOAD_GL_FUNC(glDisableVertexAttribArray, PFNGLDISABLEVERTEXATTRIBARRAYPROC);

		LOAD_GL_FUNC(glGenBuffers, PFNGLGENBUFFERSPROC);
		LOAD_GL_FUNC(glDeleteBuffers, PFNGLDELETEBUFFERSPROC);
		LOAD_GL_FUNC(glBindBuffer, PFNGLBINDBUFFERPROC);
		LOAD_GL_FUNC(glBufferData, PFNGLBUFFERDATAPROC);

		LOAD_GL_FUNC(glGenFramebuffers, PFNGLGENFRAMEBUFFERSPROC);
		LOAD_GL_FUNC(glBindFramebuffer, PFNGLBINDFRAMEBUFFERPROC);
		LOAD_GL_FUNC(glFramebufferRenderbuffer, PFNGLFRAMEBUFFERRENDERBUFFERPROC);
		LOAD_GL_FUNC(glFramebufferTexture, PFNGLFRAMEBUFFERTEXTUREPROC);
		LOAD_GL_FUNC(glFramebufferTexture2D, PFNGLFRAMEBUFFERTEXTURE2DPROC);
		LOAD_GL_FUNC(glDrawBuffers , PFNGLDRAWBUFFERSPROC);
		LOAD_GL_FUNC(glCheckFramebufferStatus , PFNGLCHECKFRAMEBUFFERSTATUSPROC);
		LOAD_GL_FUNC(glDeleteFramebuffers, PFNGLDELETEFRAMEBUFFERSPROC);

		LOAD_GL_FUNC(glCreateProgram, PFNGLCREATEPROGRAMPROC);
		LOAD_GL_FUNC(glDeleteProgram, PFNGLDELETEPROGRAMPROC);
		LOAD_GL_FUNC(glGetProgramInfoLog, PFNGLGETPROGRAMINFOLOGPROC);
		LOAD_GL_FUNC(glGetProgramiv, PFNGLGETPROGRAMIVPROC);
		LOAD_GL_FUNC(glLinkProgram, PFNGLLINKPROGRAMPROC);
		LOAD_GL_FUNC(glUseProgram, PFNGLUSEPROGRAMPROC);

		LOAD_GL_FUNC(glCreateShader, PFNGLCREATESHADERPROC);
		LOAD_GL_FUNC(glDeleteShader, PFNGLDELETESHADERPROC);
		LOAD_GL_FUNC(glAttachShader, PFNGLATTACHSHADERPROC);
		LOAD_GL_FUNC(glDetachShader, PFNGLDETACHSHADERPROC);
		LOAD_GL_FUNC(glGetShaderInfoLog, PFNGLGETSHADERINFOLOGPROC);
		LOAD_GL_FUNC(glGetShaderiv, PFNGLGETSHADERIVPROC);
		LOAD_GL_FUNC(glShaderSource, PFNGLSHADERSOURCEPROC);
		LOAD_GL_FUNC(glCompileShader, PFNGLCOMPILESHADERPROC);

		LOAD_GL_FUNC(glTexStorage2D, PFNGLTEXSTORAGE2DPROC);
		LOAD_GL_FUNC(glTextureSubImage2D, PFNGLTEXTURESUBIMAGE2DPROC);
		LOAD_GL_FUNC(glActiveTexture, PFNGLACTIVETEXTUREPROC);
		LOAD_GL_FUNC(glGenerateMipmap, PFNGLGENERATEMIPMAPPROC);

		LOAD_GL_FUNC(glGenRenderbuffers, PFNGLGENRENDERBUFFERSPROC);
		LOAD_GL_FUNC(glBindRenderbuffer, PFNGLBINDRENDERBUFFERPROC);
		LOAD_GL_FUNC(glRenderbufferStorage, PFNGLRENDERBUFFERSTORAGEPROC);
		LOAD_GL_FUNC(glRenderbufferStorageMultisample, PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC);
		LOAD_GL_FUNC(glDeleteRenderbuffers, PFNGLDELETERENDERBUFFERSPROC);

		LOAD_GL_FUNC(glGetActiveUniform, PFNGLGETACTIVEUNIFORMPROC);
		LOAD_GL_FUNC(glGetUniformLocation, PFNGLGETUNIFORMLOCATIONPROC);
		LOAD_GL_FUNC(glGetUniformfv, PFNGLGETUNIFORMFVPROC);
		LOAD_GL_FUNC(glGetUniformiv, PFNGLGETUNIFORMIVPROC);
		LOAD_GL_FUNC(glGetUniformuiv, PFNGLGETUNIFORMUIVPROC);
		LOAD_GL_FUNC(glGetUniformdv, PFNGLGETUNIFORMDVPROC);
		LOAD_GL_FUNC(glGetnUniformfv, PFNGLGETNUNIFORMFVPROC);
		LOAD_GL_FUNC(glGetnUniformiv, PFNGLGETNUNIFORMIVPROC);
		LOAD_GL_FUNC(glGetnUniformuiv, PFNGLGETNUNIFORMUIVPROC);
		LOAD_GL_FUNC(glGetnUniformdv, PFNGLGETNUNIFORMDVPROC);
		LOAD_GL_FUNC(glUniform1f, PFNGLUNIFORM1FPROC);
		LOAD_GL_FUNC(glUniform2f, PFNGLUNIFORM2FPROC);
		LOAD_GL_FUNC(glUniform3f, PFNGLUNIFORM3FPROC);
		LOAD_GL_FUNC(glUniform4f, PFNGLUNIFORM4FPROC);
		LOAD_GL_FUNC(glUniform1i, PFNGLUNIFORM1IPROC);
		LOAD_GL_FUNC(glUniform2i, PFNGLUNIFORM2IPROC);
		LOAD_GL_FUNC(glUniform3i, PFNGLUNIFORM3IPROC);
		LOAD_GL_FUNC(glUniform4i, PFNGLUNIFORM4IPROC);
		LOAD_GL_FUNC(glUniform1ui, PFNGLUNIFORM1UIPROC);
		LOAD_GL_FUNC(glUniform2ui, PFNGLUNIFORM2UIPROC);
		LOAD_GL_FUNC(glUniform3ui, PFNGLUNIFORM3UIPROC);
		LOAD_GL_FUNC(glUniform4ui, PFNGLUNIFORM4UIPROC);
		LOAD_GL_FUNC(glUniform1fv, PFNGLUNIFORM1FVPROC);
		LOAD_GL_FUNC(glUniform2fv, PFNGLUNIFORM2FVPROC);
		LOAD_GL_FUNC(glUniform3fv, PFNGLUNIFORM3FVPROC);
		LOAD_GL_FUNC(glUniform4fv, PFNGLUNIFORM4FVPROC);
		LOAD_GL_FUNC(glUniform1iv, PFNGLUNIFORM1IVPROC);
		LOAD_GL_FUNC(glUniform2iv, PFNGLUNIFORM2IVPROC);
		LOAD_GL_FUNC(glUniform3iv, PFNGLUNIFORM3IVPROC);
		LOAD_GL_FUNC(glUniform4iv, PFNGLUNIFORM4IVPROC);
		LOAD_GL_FUNC(glUniform1uiv, PFNGLUNIFORM1UIVPROC);
		LOAD_GL_FUNC(glUniform2uiv, PFNGLUNIFORM2UIVPROC);
		LOAD_GL_FUNC(glUniform3uiv, PFNGLUNIFORM3UIVPROC);
		LOAD_GL_FUNC(glUniform4uiv, PFNGLUNIFORM4UIVPROC);
		LOAD_GL_FUNC(glUniformMatrix2fv, PFNGLUNIFORMMATRIX2FVPROC);
		LOAD_GL_FUNC(glUniformMatrix3fv, PFNGLUNIFORMMATRIX3FVPROC);
		LOAD_GL_FUNC(glUniformMatrix4fv, PFNGLUNIFORMMATRIX4FVPROC);
		LOAD_GL_FUNC(glUniformMatrix2x3fv, PFNGLUNIFORMMATRIX2X3FVPROC);
		LOAD_GL_FUNC(glUniformMatrix3x2fv, PFNGLUNIFORMMATRIX3X2FVPROC);
		LOAD_GL_FUNC(glUniformMatrix2x4fv, PFNGLUNIFORMMATRIX2X4FVPROC);
		LOAD_GL_FUNC(glUniformMatrix4x2fv, PFNGLUNIFORMMATRIX4X2FVPROC);
		LOAD_GL_FUNC(glUniformMatrix3x4fv, PFNGLUNIFORMMATRIX3X4FVPROC);
		LOAD_GL_FUNC(glUniformMatrix4x3fv, PFNGLUNIFORMMATRIX4X3FVPROC);
	}
}
