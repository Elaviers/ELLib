#include "Mesh_Static.hpp"
#include "IO.hpp"

Mesh_Static::Mesh_Static(const Buffer<Vertex17F>& vertices, const Buffer<uint32>& elements) : Mesh(&_box), vertices(vertices), elements(elements)
{
}

void Mesh_Static::UpdateRenderer()
{
	_renderer.GL().Create(vertices.begin(), (GLsizei)vertices.GetSize(), elements.begin(), (GLsizei)elements.GetSize());
}

enum class EEFileVersion
{
	V1 = 1
};

constexpr byte CURRENT_FILE_VERSION = (byte)EEFileVersion::V1;

void Mesh_Static::Read(ByteReader& reader)
{
	reader.IncrementIndex(1); //Skip type

	byte version = reader.Read_byte();

	if (version == (byte)EEFileVersion::V1)
	{
		uint32 vertCount = reader.Read_uint32();

		vertices.SetSize(vertCount);

		for (uint32 i = 0; i < vertCount; ++i)
		{
			vertices[i].pos.Read(reader);
			vertices[i].normal.Read(reader);
			vertices[i].uv.Read(reader);

			if ((i + 1) % 3 == 0)
				Vertex17F::CalculateTangents(vertices[i - 2], vertices[i - 1], vertices[i - 2]);
		}

		uint32 elemCount = reader.Read_uint32();

		elements.SetSize(elemCount);

		for (uint32 i = 0; i < elemCount; ++i)
		{
			elements[i] = reader.Read_uint32();

			if ((i + 1) % 3 == 0)
				Vertex17F::CalculateTangents(vertices[elements[i - 2]], vertices[elements[i - 1]], vertices[elements[i]]);
		}
		
		_box.SetTransform(reader.Read<Transform>());

		UpdateRenderer();
	}
}

void Mesh_Static::Write(ByteWriter& writer) const
{
	writer.EnsureSpace(
		1 +													//Type (Static)
		1 +													//Version
		4 +													//Vert count
		(4 * (3+3+2)) * vertices.GetSize() +				//Vertices
		4 +													//Element count
		4 * elements.GetSize() +							//Elements
		3 * 4 * 3											//Bounds
	);

	writer.Write_byte(ASSET_MESH_STATIC);
	writer.Write_byte(CURRENT_FILE_VERSION);

	writer.Write_uint32((uint32)vertices.GetSize());

	for (size_t i = 0; i < vertices.GetSize(); ++i)
	{
		const Vertex17F& v = vertices[i];

		v.pos.Write(writer);
		v.normal.Write(writer);
		v.uv.Write(writer);
	}

	writer.Write_uint32((uint32)elements.GetSize());

	for (size_t i = 0; i < elements.GetSize(); ++i)
		writer.Write_uint32(elements[i]);

	_box.GetTransform().Write(writer);
}
