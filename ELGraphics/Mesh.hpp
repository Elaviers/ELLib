#pragma once
#include "MeshRenderer.hpp"
#include <ELCore/Asset.hpp>
#include <ELCore/Concepts.hpp>
#include <ELMaths/Volume.hpp>

class Mesh : public Asset
{
protected:
	MeshRenderer _renderer;

	Volume* _volume;

	Mesh(Volume* volume) : _volume(volume) {}

public:
	virtual ~Mesh() {}

	const Volume* GetVolume() const { return _volume; }

	void SetRenderMode(ERenderMode mode) { _renderer.SetRenderMode(mode); }
	void Render() const { _renderer.Render(); }
	
	virtual bool IsValid() const { return false; }
	virtual void UpdateRenderer() {}

	static Mesh* FromData(const Buffer<byte>&);
};
