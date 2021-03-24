#pragma once
#include <ELSys/GL.hpp>
#include <ELMaths/Matrix4.hpp>
#include <ELMaths/Vector2.hpp>
#include <ELMaths/Vector3.hpp>
#include <ELMaths/Vector4.hpp>

namespace DefaultUniformVars
{
	constexpr const char *mat4Projection = "M_Projection";
	constexpr const char *mat4View = "M_View";
	constexpr const char *mat4Model = "M_Model";

	constexpr const char *mat4aBones = "Bones";

	constexpr const char *vec4Colour = "Colour";
	constexpr const char *uvec4Colour = "uColour";

	constexpr const char *vec2UVOffset = "UVOffset";
	constexpr const char *vec2UVScale = "UVScale";

	constexpr const char *intTextureDiffuse = "T_Diffuse";
	constexpr const char *intTextureNormal = "T_Normal";
	constexpr const char *intTextureSpecular = "T_Specular";
	constexpr const char *intTextureReflection = "T_Reflection";

	constexpr const char *boolBlend = "Blend";
	constexpr const char *vec4BlendPrimary = "BlendPrimary";
	constexpr const char *vec4BlendSecondary = "BlendSecondary";
	constexpr const char *vec4BlendTertiary = "BlendTertiary";

	constexpr const char *intCubemap = "Cubemap";
	constexpr const char *intSkybox = "Skybox";

	constexpr const char *boolIsFont = "IsFont";

	constexpr const char *intLightCount = "LightCount";
}

class GLProgram
{
private:
	GLuint _id;

	int _maxLightCount;

	static const GLProgram* _currentProgram;

public:
	GLProgram() : _id(0), _maxLightCount(0) {}
	GLProgram(const GLProgram&) = delete;
	GLProgram(GLProgram&& other) noexcept : _id(other._id), _maxLightCount(other._maxLightCount) { other._id = 0; other._maxLightCount = 0; }
	~GLProgram() {}

	static const GLProgram& Current() { return *_currentProgram; }

	GLuint GetID() const { return _id; }

	void Create(const char *vertSource, const char *fragSource);
	void Load(const char *vertFile, const char *fragFile);

	void Use() const { glUseProgram(_id); _currentProgram = this; }
	GLint GetUniformLocation(const char *name) const;

	GLProgram& operator=(GLProgram&& other) noexcept { _id = other._id; other._id = 0; }

	//Setter inlines

	void SetBool(const char* name, bool x) const					{ SetInt(name, x ? 1 : 0); }
	void SetInt(const char *name, GLint x) const					{ glUniform1i(GetUniformLocation(name), x); }
	void SetFloat(const char* name, GLfloat x) const				{ glUniform1f(GetUniformLocation(name), x); }
	void SetMatrix4(const char *name, const Matrix4 &x) const		{ glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &x[0][0]); }
	void SetVec2(const char *name, const Vector2 &x) const			{ glUniform2fv(GetUniformLocation(name), 1, x.GetData()); }
	void SetVec3(const char *name, const Vector3 &x) const			{ glUniform3fv(GetUniformLocation(name), 1, x.GetData()); }
	void SetVec4(const char *name, const Vector4 &x) const			{ glUniform4fv(GetUniformLocation(name), 1, x.GetData()); }
	void SetUVec4(const char* name, const uint32 x[4]) const		{ glUniform4uiv(GetUniformLocation(name), 1, x); }

	bool GetBool(const char* name) const { return GetInt(name) != 0; }
	GLint GetInt(const char* name) const { GLint x; glGetUniformiv(_id, GetUniformLocation(name), &x); return x; }
	GLfloat GetFloat(const char* name) const { GLfloat x; glGetUniformfv(_id, GetUniformLocation(name), &x); return x; }
	Matrix4 GetMatrix4(const char* name) const { GLfloat x[16]; glGetUniformfv(_id, GetUniformLocation(name), x); return Matrix4(x[0], x[1], x[2], x[3], x[4], x[5], x[6], x[7], x[8], x[9], x[10], x[11], x[12], x[13], x[14], x[15]); }
	Vector2 GetVec2(const char* name) const { GLfloat x[2]; glGetUniformfv(_id, GetUniformLocation(name), x); return Vector2(x[0], x[1]); }
	Vector3 GetVec3(const char* name) const { GLfloat x[3]; glGetUniformfv(_id, GetUniformLocation(name), x); return Vector3(x[0], x[1], x[2]); }
	Vector4 GetVec4(const char* name) const { GLfloat x[4]; glGetUniformfv(_id, GetUniformLocation(name), x); return Vector4(x[0], x[1], x[2], x[3]); }
};
