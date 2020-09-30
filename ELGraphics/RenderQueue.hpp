#pragma once
#include "Colour.hpp"
#include "MeshManager.hpp"
#include "TextureManager.hpp"
#include "RenderEntry.hpp"
#include <ELCore/Concepts.hpp>
#include <ELCore/Pool.hpp>
#include <ELMaths/Transform.hpp>

class Projection;

struct QueueGroup
{
	const int priority;
	List<const RenderEntry*> queue;

	QueueGroup(
		int priority,
		const NewHandler& newHandler,
		const DeleteHandler& deleteHandler)
		: priority(priority), queue(newHandler, deleteHandler)
	{}
};

class RenderQueue
{
private:
	typedef MultiPool<byte, sizeof(RenderEntry) * 32> _EntryPoolType;
	typedef MultiPool<byte, sizeof(const RenderEntry*) * 32> _PtrPoolType;

	_EntryPoolType _entryPool;
	_PtrPoolType _ptrPool;

	List<RenderEntry> _entries;
	List<QueueGroup> _queues;
	
	List<const RenderEntry*>& _GetQueue(int priority);

public:
	RenderQueue() : _entries(NewHandler(&_entryPool, &_EntryPoolType::NewArray), DeleteHandler(&_entryPool, &_EntryPoolType::DeleteHandler)) {}
	~RenderQueue() {}

	void Clear();
	void Render(ERenderChannels channels, const MeshManager& meshManager, const TextureManager& textureManager, int lightCount) const;

	void AddEntry(const RenderEntry*, int priority = 0);
	RenderEntry& CreateEntry(ERenderChannels renderChannels, int priority = 0);
};
