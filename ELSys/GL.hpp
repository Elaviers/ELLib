#pragma once
#include <Windows.h>
#include <gl/GL.h>
#include <gl/glext.h>
#include <gl/wglext.h>

class Window;

extern PFNGLBLENDEQUATIONPROC glBlendEquation;
extern PFNGLBLENDEQUATIONSEPARATEPROC glBlendEquationSeparate;
extern PFNGLBLENDFUNCSEPARATEPROC glBlendFuncSeparate;
extern PFNGLBINDSAMPLERPROC glBindSampler;
extern PFNGLDRAWELEMENTSBASEVERTEXPROC glDrawElementsBaseVertex;
extern PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation;

//Windows
extern PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;
extern PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB;
extern PFNWGLSWAPINTERVALEXTPROC	wglSwapIntervalEXT;

//Vertex Arrays
extern PFNGLGENVERTEXARRAYSPROC		glGenVertexArrays;
extern PFNGLDELETEVERTEXARRAYSPROC	glDeleteVertexArrays;
extern PFNGLBINDVERTEXARRAYPROC		glBindVertexArray;

extern PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
extern PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
extern PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;

//Buffers
extern PFNGLGENBUFFERSPROC			glGenBuffers;
extern PFNGLDELETEBUFFERSPROC		glDeleteBuffers;
extern PFNGLBINDBUFFERPROC			glBindBuffer;
extern PFNGLBUFFERDATAPROC			glBufferData;

//Frame Buffers
extern PFNGLGENFRAMEBUFFERSPROC		glGenFramebuffers;
extern PFNGLBINDFRAMEBUFFERPROC		glBindFramebuffer;
extern PFNGLFRAMEBUFFERRENDERBUFFERPROC glFramebufferRenderbuffer;
extern PFNGLFRAMEBUFFERTEXTUREPROC	glFramebufferTexture;
extern PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D;
extern PFNGLDRAWBUFFERSPROC			glDrawBuffers;
extern PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus;
extern PFNGLDELETEFRAMEBUFFERSPROC	glDeleteFramebuffers;

//Programs
extern PFNGLCREATEPROGRAMPROC		glCreateProgram;
extern PFNGLDELETEPROGRAMPROC		glDeleteProgram;
extern PFNGLGETPROGRAMINFOLOGPROC	glGetProgramInfoLog;
extern PFNGLGETPROGRAMIVPROC		glGetProgramiv;
extern PFNGLLINKPROGRAMPROC			glLinkProgram;
extern PFNGLUSEPROGRAMPROC			glUseProgram;

//Shaders
extern PFNGLCREATESHADERPROC		glCreateShader;
extern PFNGLDELETESHADERPROC		glDeleteShader;
extern PFNGLATTACHSHADERPROC		glAttachShader;
extern PFNGLDETACHSHADERPROC		glDetachShader;
extern PFNGLGETSHADERINFOLOGPROC	glGetShaderInfoLog;
extern PFNGLGETSHADERIVPROC			glGetShaderiv;
extern PFNGLSHADERSOURCEPROC		glShaderSource;
extern PFNGLCOMPILESHADERPROC		glCompileShader;

//Textures
extern PFNGLTEXSTORAGE2DPROC		glTexStorage2D;
extern PFNGLTEXTURESUBIMAGE2DPROC	glTextureSubImage2D;
extern PFNGLACTIVETEXTUREPROC		glActiveTexture;
extern PFNGLGENERATEMIPMAPPROC		glGenerateMipmap;

//Render Buffers
extern PFNGLGENRENDERBUFFERSPROC	glGenRenderbuffers;
extern PFNGLBINDRENDERBUFFERPROC	glBindRenderbuffer;
extern PFNGLRENDERBUFFERSTORAGEPROC glRenderbufferStorage;
extern PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC glRenderbufferStorageMultisample;
extern PFNGLDELETERENDERBUFFERSPROC	glDeleteRenderbuffers;

//Uniform variables
extern PFNGLGETACTIVEUNIFORMPROC	glGetActiveUniform;
extern PFNGLGETUNIFORMLOCATIONPROC	glGetUniformLocation;
extern PFNGLGETUNIFORMFVPROC		glGetUniformfv;
extern PFNGLGETUNIFORMIVPROC		glGetUniformiv;
extern PFNGLGETUNIFORMUIVPROC		glGetUniformuiv;
extern PFNGLGETUNIFORMDVPROC		glGetUniformdv;
extern PFNGLGETNUNIFORMFVPROC		glGetnUniformfv;
extern PFNGLGETNUNIFORMIVPROC		glGetnUniformiv;
extern PFNGLGETNUNIFORMUIVPROC		glGetnUniformuiv;
extern PFNGLGETNUNIFORMDVPROC		glGetnUniformdv;
extern PFNGLUNIFORM1FPROC			glUniform1f;
extern PFNGLUNIFORM2FPROC			glUniform2f;
extern PFNGLUNIFORM3FPROC			glUniform3f;
extern PFNGLUNIFORM4FPROC			glUniform4f;
extern PFNGLUNIFORM1IPROC			glUniform1i;
extern PFNGLUNIFORM2IPROC			glUniform2i;
extern PFNGLUNIFORM3IPROC			glUniform3i;
extern PFNGLUNIFORM4IPROC			glUniform4i;
extern PFNGLUNIFORM1UIPROC			glUniform1ui;
extern PFNGLUNIFORM2UIPROC			glUniform2ui;
extern PFNGLUNIFORM3UIPROC			glUniform3ui;
extern PFNGLUNIFORM4UIPROC			glUniform4ui;
extern PFNGLUNIFORM1FVPROC			glUniform1fv;
extern PFNGLUNIFORM2FVPROC			glUniform2fv;
extern PFNGLUNIFORM3FVPROC			glUniform3fv;
extern PFNGLUNIFORM4FVPROC			glUniform4fv;
extern PFNGLUNIFORM1IVPROC			glUniform1iv;
extern PFNGLUNIFORM2IVPROC			glUniform2iv;
extern PFNGLUNIFORM3IVPROC			glUniform3iv;
extern PFNGLUNIFORM4IVPROC			glUniform4iv;
extern PFNGLUNIFORM1UIVPROC			glUniform1uiv;
extern PFNGLUNIFORM2UIVPROC			glUniform2uiv;
extern PFNGLUNIFORM3UIVPROC			glUniform3uiv;
extern PFNGLUNIFORM4UIVPROC			glUniform4uiv;
extern PFNGLUNIFORMMATRIX2FVPROC	glUniformMatrix2fv;
extern PFNGLUNIFORMMATRIX3FVPROC	glUniformMatrix3fv;
extern PFNGLUNIFORMMATRIX4FVPROC	glUniformMatrix4fv;
extern PFNGLUNIFORMMATRIX2X3FVPROC	glUniformMatrix2x3fv;
extern PFNGLUNIFORMMATRIX3X2FVPROC	glUniformMatrix3x2fv;
extern PFNGLUNIFORMMATRIX2X4FVPROC	glUniformMatrix2x4fv;
extern PFNGLUNIFORMMATRIX4X2FVPROC	glUniformMatrix4x2fv;
extern PFNGLUNIFORMMATRIX3X4FVPROC	glUniformMatrix3x4fv;
extern PFNGLUNIFORMMATRIX4X3FVPROC	glUniformMatrix4x3fv;

namespace GL
{
	void LoadDummyExtensions();
	void LoadExtensions(const Window&);
}
