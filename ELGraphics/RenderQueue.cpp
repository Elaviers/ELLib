#include "RenderQueue.hpp"
#include "RenderCommand.hpp"
#include <ELSys/GLProgram.hpp>

List<const RenderEntry*>& RenderQueue::_GetQueue(int priority)
{
	for (auto it = _queues.begin(); it.IsValid(); ++it)
	{
		if (it->priority > priority)
			return _queues.Insert(
				it, 
				priority,
				NewHandler(&_ptrPool, &_PtrPoolType::NewArray), 
				DeleteHandler(&_ptrPool, &_PtrPoolType::DeleteHandler))->queue;

		if (it->priority == priority)
			return it->queue;
	}

	return _queues.Emplace(priority, NewHandler(&_ptrPool, &_PtrPoolType::NewArray), DeleteHandler(&_ptrPool, &_PtrPoolType::DeleteHandler))->queue;
}

void RenderQueue::Clear()
{
	_entries.Clear();

	for (QueueGroup& q : _queues)
		q.queue.Clear();

	_entryPool.Clear();
	_ptrPool.Clear();
}

void RenderQueue::Render(ERenderChannels channels, const MeshManager* meshManager, const TextureManager* textureManager, int lightCount) const
{
	const float zero = 1.f;
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
	_GetQueue(priority).Add(entry);
}

RenderEntry& RenderQueue::CreateEntry(ERenderChannels renderChannels, int priority)
{
	RenderEntry& entry = *_entries.Emplace(renderChannels, NewHandler(&_entryPool, &_EntryPoolType::NewArray), DeleteHandler(&_entryPool, &_EntryPoolType::DeleteHandler));
	_GetQueue(priority).Emplace(&entry);
	return entry;
}
