#include "Mesh_Skeletal.hpp"
#include "IO.hpp"

void Mesh_Skeletal::UpdateRenderer()
{
	_renderer.GL().Create(vertices.begin(), (GLsizei)vertices.GetSize(), elements.begin(), (GLsizei)elements.GetSize());
}

enum class EEFileVersion
{
	V1 = 1
};

constexpr byte CURRENT_FILE_VERSION = (byte)EEFileVersion::V1;

void Mesh_Skeletal::Read(ByteReader& reader)
{
	reader.IncrementIndex(1); //Skip type

	byte version = reader.Read_byte();

	if (version == (byte)EEFileVersion::V1)
	{
		byte bonesPerVertex = reader.Read_byte();
		if (bonesPerVertex != VertexSkeletal::BONE_COUNT)
		{
			Debug::Error("I can't be bothered to load this mesh because it does not have 2 bones per vertex");
			return;
		}

		uint32 vertCount = reader.Read_uint32();

		vertices.SetSize(vertCount);

		for (uint32 i = 0; i < vertCount; ++i)
		{
			vertices[i].pos.Read(reader);
			vertices[i].normal.Read(reader);
			vertices[i].uv.Read(reader);

			for (int j = 0; j < bonesPerVertex; ++j)
			{
				vertices[i].boneIndices[j] = reader.Read_uint32();
				vertices[i].boneWeights[j] = reader.Read_float();
			}
		}

		uint32 elemCount = reader.Read_uint32();

		elements.SetSize(elemCount);

		for (uint32 i = 0; i < elemCount; ++i)
		{
			elements[i] = reader.Read_uint32();

			if ((i + 1) % 3 == 0)
				VertexSkeletal::CalculateTangents(vertices[elements[i - 2]], vertices[elements[i - 1]], vertices[elements[i]]);
		}

		uint32 jointCount = reader.Read_uint32();

		for (size_t i = 0; i < jointCount; ++i)
		{
			Joint *parent = skeleton.GetJointWithID(reader.Read_uint32());

			Joint* j = skeleton.CreateJoint(parent);
			j->name.Read(reader);
			j->bindingMatrix.Read(reader);
		}

		_box.SetTransform(reader.Read<Transform>());

		UpdateRenderer();
	}
}

void Mesh_Skeletal::Write(ByteWriter& writer) const
{
	uint32 jointCount = (uint32)skeleton.GetJointCount();

	Buffer<const Joint*> jointBuffer;
	jointBuffer.SetSize(jointCount);

	uint32 jointId = 0;

	size_t jointDataSize = 0;

	for (auto it = skeleton.FirstListElement(); it; ++it)
	{
		jointBuffer[jointId++] = &*it;

		//ParentID + name + binding
		jointDataSize += 4 + (it->name.GetLength() + 1) + (4 * 4 * 4);
	}

	writer.EnsureSpace(
		1 +													//Type (Skeletal)
		1 +													//Version
		1 +													//Bone indices/weights per vertex (2)
		4 +													//Vert count
		(4 * (3+2+3+3+3+3) + ((4 + 4) * VertexSkeletal::BONE_COUNT)) * vertices.GetSize() +	//Vertices
		4 +													//Element count
		4 * elements.GetSize() +							//Elements
		4 +													//Joint count
		jointDataSize +										//Joints
		4 * 3 +												//Bounds min
		4 * 3);												//Bounds max

	writer.Write_byte(ASSET_MESH_SKELETAL);
	writer.Write_byte(CURRENT_FILE_VERSION);
	writer.Write_byte(VertexSkeletal::BONE_COUNT);

	writer.Write_uint32((uint32)vertices.GetSize());

	for (size_t i = 0; i < vertices.GetSize(); ++i)
	{
		const VertexSkeletal& v = vertices[i];

		v.pos.Write(writer);
		v.normal.Write(writer);
		v.uv.Write(writer);
		
		for (int j = 0; j < VertexSkeletal::BONE_COUNT; ++j)
		{
			writer.Write_uint32(v.boneIndices[j]);
			writer.Write_float(v.boneWeights[j]);
		}
	}

	writer.Write_uint32((uint32)elements.GetSize());

	for (size_t i = 0; i < elements.GetSize(); ++i)
		writer.Write_uint32(elements[i]);

	writer.Write_uint32((uint32)jointBuffer.GetSize());

	for (size_t i = 0; i < jointBuffer.GetSize(); ++i)
	{
		const Joint* joint = jointBuffer[i];

		size_t parentId = 0;
		for (size_t id = 0; id < jointBuffer.GetSize(); ++id)
		{
			if (joint->GetParent() == jointBuffer[id])
			{
				parentId = id;
				break;
			}
		}

		writer.Write_uint32((uint32)parentId);
		writer.Write_cstr(joint->name.begin());
		joint->bindingMatrix.Write(writer);
	}

	_box.GetTransform().Write(writer);
}
