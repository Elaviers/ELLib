#pragma once
#include "ERenderChannels.hpp"
#include "RenderCommand.hpp"
#include <ELCore/Allocator.hpp>
#include <ELCore/Axes.hpp>
#include <ELCore/Concepts.hpp>
#include <ELCore/List.hpp>
#include <ELCore/PagedMemory.hpp>
#include <ELMaths/Vector3.hpp>

class Projection;
class Transform;

class RenderEntry
{
public:
	template <typename T>
	using AllocatorType = PagedMemory<>::Allocator<T>;

protected:
	struct _Command
	{
		const RenderCommand* command;
		bool bDelete;

		constexpr _Command(const RenderCommand* command, bool bDelete) noexcept : command(command), bDelete(bDelete) {}
	};

	List<_Command, AllocatorType<_Command>> _commands;
	ERenderChannels _renderChannels;
	AllocatorType<RenderCommand> _allocator;
	
	template <typename T, typename ...ARGS>
	requires Concepts::DerivedFrom<T, RenderCommand>
		T& _AddCommand(ARGS&&... args)
	{
		void* mem = ((AllocatorType<T>&)_allocator).allocate(1);
		T* cmd = new (mem) T(std::forward<ARGS>(args)...);
		_commands.EmplaceBack(cmd, true);
		return *cmd;
	}

public:
	constexpr RenderEntry(ERenderChannels renderChannels, const AllocatorType<RenderCommand>& allocator) : 
		_commands(allocator), _renderChannels(renderChannels), _allocator(allocator) {}
	
	RenderEntry(ERenderChannels renderChannels);

	RenderEntry(const RenderEntry&) = delete;
	~RenderEntry();

	RenderEntry& operator=(const RenderEntry&) = delete;
	
	const ERenderChannels& GetRenderChannels() const { return _renderChannels; }

	void Clear();

	void Render(RenderContext&) const;

	constexpr void AddCommand(const RenderCommand& command)
	{
		_commands.EmplaceBack(&command, false);
	}

	//This whole block isn't really neccessary.
	//Originally, _AddCommand was public and I just used that, but type safety is enforced better this way...
	//_AddCommand will need to be exposed if I ever need to define RenderCommands outside of this project though....... :/

	RCMDSetBool& AddSetIsFont(bool isFont) { return _AddCommand<RCMDSetBool>(RCMDSetBool::Type::FONT, isFont); }
	RCMDSetFloat& AddSetLineWidth(float lineWidth) { return _AddCommand<RCMDSetFloat>(RCMDSetFloat::Type::LINE_WIDTH, lineWidth); }
	RCMDSetUVec4& AddSetUVec4(RCMDSetUVec4::Type type, const uint32 uvec[4]) { return _AddCommand<RCMDSetUVec4>(type, uvec); }
	RCMDSetMat4& AddSetMat4(RCMDSetMat4::Type type, const Matrix4& matrix) { return _AddCommand<RCMDSetMat4>(type, matrix); }
	RCMDSetMat4& AddSetMat4(RCMDSetMat4::Type type, Matrix4&& matrix) { return _AddCommand<RCMDSetMat4>(type, matrix); }
	RCMDSetUVOffset& AddSetUVOffset(const Vector2& uvOffset) { return _AddCommand<RCMDSetUVOffset>(uvOffset); }
	RCMDSetUVOffset& AddSetUVOffset(Vector2&& uvOffset = Vector2()) { return _AddCommand<RCMDSetUVOffset>(uvOffset); }
	RCMDSetUVScale& AddSetUVScale(const Vector2& uvScale) { return _AddCommand<RCMDSetUVScale>(uvScale); }
	RCMDSetUVScale& AddSetUVScale(Vector2&& uvScale = Vector2(1.f, 1.f)) { return _AddCommand<RCMDSetUVScale>(uvScale); }
	RCMDSetColour& AddSetColour(const Colour& colour = Colour::White) { return _AddCommand<RCMDSetColour>(colour); }
	RCMDSetColour& AddPushColourOverride(const Colour& colour = Colour::White) { return _AddCommand<RCMDSetColour>(colour, true); }
	RCMDSetColour& AddSetColour(const Colour& primary, const Colour& secondary, const Colour& tertiary) { return _AddCommand<RCMDSetColour>(primary, secondary, tertiary); }
	RCMDSetColour& AddPushColourOverride(const Colour& primary, const Colour& secondary, const Colour& tertiary) { return _AddCommand<RCMDSetColour>(primary, secondary, tertiary, true); }
	RCMDPopColourOverride& AddPopColourOverride() { return _AddCommand<RCMDPopColourOverride>(); }
	RCMDSetTexture& AddSetTexture(const Texture& texture, uint16 unit) { return _AddCommand<RCMDSetTexture>(texture, unit); }
	RCMDSetTexture& AddSetTextureGL(GLuint tex, uint16 unit) { return _AddCommand<RCMDSetTexture>(tex, unit); }
	RCMDSetTexture& AddSetTexture(RCMDSetTexture::Type type, uint16 unit) { return _AddCommand<RCMDSetTexture>(type, unit); }
	RCMDSetCubemap& AddSetCubemap(const GLCubemap& cubemap, uint16 unit) { return _AddCommand<RCMDSetCubemap>(cubemap, unit); }
	RCMDSetCubemap& AddSetCubemapGL(GLuint tex, uint16 unit) { return _AddCommand<RCMDSetCubemap>(tex, unit); }
	RCMDRenderMesh& AddRenderMesh(const Mesh& mesh) { return _AddCommand<RCMDRenderMesh>(mesh); }
	RCMDSetSkinningMatrices& AddSetSkinningMatrices(const Buffer<Matrix4>& matrices) { return _AddCommand<RCMDSetSkinningMatrices>(matrices); }
	RCMDSetSkinningMatrices& AddSetSkinningMatrices(const Buffer<Matrix4>* matrices) { return _AddCommand<RCMDSetSkinningMatrices>(matrices); }
	RCMDLight& AddLight(const Vector3& position, const Vector3& colour, float radius) { return _AddCommand<RCMDLight>(position, colour, radius); }
	RCMDViewport& AddSetViewport(int32 x, int32 y, int32 width, int32 height) { return _AddCommand<RCMDViewport>(x, y, width, height); }

	//Utility
	inline RCMDSetMat4& AddSetTransform(const Matrix4& transform) { return _AddCommand<RCMDSetMat4>(RCMDSetMat4::Type::TRANSFORM, transform); }
	inline RCMDSetMat4& AddSetTransform(Matrix4&& transform) { return _AddCommand<RCMDSetMat4>(RCMDSetMat4::Type::TRANSFORM, transform); }

	void AddLine(const Vector3& a, const Vector3& b);
	void AddBox(const Vector3& p1, const Vector3& p2);
	void AddRing(const Vector3& centre, const Vector3& nomal, float radius, int segments);
	void AddGrid(const Transform& cameraTransform, const Projection& cameraProjection, EAxis plane, float gap, float limit, float offset, float z);
};
