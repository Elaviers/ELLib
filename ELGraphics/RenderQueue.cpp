#include "RenderQueue.hpp"
#include "RenderCommand.hpp"
#include <ELSys/GLProgram.hpp>

QueueGroup& RenderQueue::_GetQueue(int priority)
{
	for (auto it = _queues.begin(); it.IsValid(); ++it)
	{
		if (it->priority > priority)
			return *_queues.Insert(
				it, 
				priority, 
				NewHandler(&_staticPtrPool, &_PtrPoolType::NewArray), 
				DeleteHandler(&_staticPtrPool, &_PtrPoolType::DeleteHandler), 
				NewHandler(&_dynamicPtrPool, &_PtrPoolType::NewArray), 
				DeleteHandler(&_dynamicPtrPool, &_PtrPoolType::DeleteHandler));

		if (it->priority == priority)
			return *it;
	}

	return *_queues.Emplace(priority, NewHandler(&_staticPtrPool, &_PtrPoolType::NewArray), DeleteHandler(&_staticPtrPool, &_PtrPoolType::DeleteHandler),
		NewHandler(&_dynamicPtrPool, &_PtrPoolType::NewArray), DeleteHandler(&_dynamicPtrPool, &_PtrPoolType::DeleteHandler));
}

void RenderQueue::ClearDynamicQueue()
{
	_dynamicEntries.Clear();

	for (QueueGroup& q : _queues)
		q.dynamicQueue.Clear();

	_dynamicEntryPool.Clear();
	_dynamicPtrPool.Clear();
}

void RenderQueue::Render(ERenderChannels channels, const MeshManager& meshManager, const TextureManager& textureManager, int lightCount) const
{
	const float zero = 1.f;
	for (int i = 0; i < lightCount; ++i)
	{
		glUniform1fv(GLProgram::Current().GetUniformLocation(CSTR("Lights[", i, "].Radius")), 1, &zero);
	}

	RenderContext ctx = { &meshManager, &textureManager, 0 };

	for (const QueueGroup& q : _queues)
	{
		for (const RenderEntry* entry : q.staticQueue)
			if ((entry->GetRenderChannels() & channels) != ERenderChannels::NONE)
				entry->Render(ctx);

		for (const RenderEntry* entry : q.dynamicQueue)
			if ((entry->GetRenderChannels() & channels) != ERenderChannels::NONE)
				entry->Render(ctx);
	}
}


void RenderQueue::AddStatic(const RenderEntry* entry, int priority)
{
	QueueGroup& group = _GetQueue(priority);
	if (!group.staticQueue.Contains(entry))
		group.staticQueue.Add(entry);
}

void RenderQueue::RemoveStatic(const RenderEntry* entry, int priority)
{
	_GetQueue(priority).staticQueue.Remove(entry);
}

void RenderQueue::AddDynamic(const RenderEntry* entry, int priority)
{
	_GetQueue(priority).dynamicQueue.Add(entry);
}

RenderEntry& RenderQueue::NewDynamicEntry(ERenderChannels renderChannels, int priority)
{
	RenderEntry& entry = *_dynamicEntries.Emplace(renderChannels, NewHandler(&_dynamicEntryPool, &_EntryPoolType::NewArray), DeleteHandler(&_dynamicEntryPool, &_EntryPoolType::DeleteHandler));
	_GetQueue(priority).dynamicQueue.Emplace(&entry);
	return entry;
}
