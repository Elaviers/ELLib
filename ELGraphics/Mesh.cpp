#include "Mesh.hpp"
#include "Mesh_Static.hpp"
#include "Mesh_Skeletal.hpp"

//static
Mesh* Mesh::FromData(const Buffer<byte>& data)
{
	Mesh* mesh = nullptr;

	ByteReader reader = data;
	switch (reader.Peek_byte())
	{
	case ASSET_MESH_STATIC:
		mesh = new Mesh_Static();
		break;
	case ASSET_MESH_SKELETAL:
		mesh = new Mesh_Skeletal();
		break;
	}

	if (mesh)
	{
		mesh->Read(reader);
	}

	return mesh;
}
