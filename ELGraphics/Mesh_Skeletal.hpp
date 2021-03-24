#pragma once
#include "Mesh.hpp"
#include "Skeleton.hpp"
#include <ELCore/Buffer.hpp>
#include <ELSys/Vertex.hpp>

class Mesh_Skeletal : public Mesh
{
private:
	VBox _box;

public:
	Mesh_Skeletal() : Mesh(&_box) {}
	virtual ~Mesh_Skeletal() {}

	VBox& BoundingBox() { return _box; }
	const VBox& BoundingBox() const { return _box; }

	//todo- no public member variables here
	Buffer<VertexSkeletal> vertices;
	Buffer<uint32> elements;
	Skeleton skeleton;

	virtual bool IsValid() const { return vertices.GetSize() != 0; }

	virtual void Read(ByteReader& data) override;
	virtual void Write(ByteWriter&) const override;

	virtual void UpdateRenderer() override;
};
