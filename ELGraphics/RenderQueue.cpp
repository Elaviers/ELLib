#include "RenderQueue.hpp"
#include "RenderCommand.hpp"
#include <ELMaths/Projection.hpp>
#include <ELSys/GLProgram.hpp>

RenderQueue::EntryPtrListType& RenderQueue::_GetQueue(int priority)
{
	for (auto it = _queues.begin(); it; ++it)
	{
		if (it->priority > priority)
			return _queues.Emplace(it, priority, _memory.GetAllocator<RenderEntry*>()).queue;

		if (it->priority == priority)
			return it->queue;
	}

	return _queues.EmplaceBack(priority, _memory.GetAllocator<RenderEntry*>()).queue;
}

void RenderQueue::Clear()
{
	_entries.Clear();

	for (QueueGroup& q : _queues)
		q.queue.Clear();

	_memory.Clear();
}

void RenderQueue::Render(ERenderChannels channels, const MeshManager* meshManager, const TextureManager* textureManager, int lightCount) const
{
	const float zero = 0.f;
	for (int i = 0; i < lightCount; ++i)
		glUniform1fv(GLProgram::Current().GetUniformLocation(CSTR("Lights[", i, "].Radius")), 1, &zero);

	RenderContext ctx = { meshManager, textureManager, 0, false };

	for (const QueueGroup& q : _queues)
		for (const RenderEntry* entry : q.queue)
			if ((entry->GetRenderChannels() & channels) != ERenderChannels::NONE)
				entry->Render(ctx);
}

void RenderQueue::AddEntry(const RenderEntry* entry, int priority)
{
	_GetQueue(priority).AddBack(entry);
}

RenderEntry& RenderQueue::CreateEntry(ERenderChannels renderChannels, int priority)
{
	RenderEntry& entry = _entries.EmplaceBack(renderChannels, _memory.GetAllocator<RenderEntry>());
	_GetQueue(priority).EmplaceBack(&entry);
	return entry;
}

RenderEntry& RenderQueue::CreateCameraEntry(const Projection& projection, const Transform& transform, ERenderChannels renderChannels, int priority)
{
	RenderEntry& e = CreateEntry(renderChannels, priority);
	e.AddSetViewport(0, 0, projection.GetDimensions().x, projection.GetDimensions().y);
	e.AddSetMat4(RCMDSetMat4::Type::VIEW, transform.GetInverseMatrix());
	e.AddSetMat4(RCMDSetMat4::Type::PROJECTION, projection.GetMatrix());
	return e;
}
