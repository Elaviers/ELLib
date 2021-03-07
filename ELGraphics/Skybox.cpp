#include "Skybox.hpp"
#include "IO.hpp"
#include "RenderCommand.hpp"
#include "RenderQueue.hpp"
#include "TextureManager.hpp"
#include <ELCore/Context.hpp>
#include <ELSys/Debug.hpp>
#include <utility> //move

Skybox::Skybox()
{
}


Skybox::~Skybox()
{
}

void Skybox::Load(const char *faceFilenames[6], const Context& context)
{
	TextureManager* textureManager = context.GetPtr<TextureManager>();
	if (textureManager == nullptr) Debug::Error("Skybox::Load called without textureManager in context");

	Buffer<byte> facebuffers[6];
	TextureData faces[6] = {};

	for (int i = 0; i < 6; ++i) {
		bool success = IO::ReadPNGFile(CSTR(textureManager->GetRootPath(), faceFilenames[i]), facebuffers[i], faces[i].width, faces[i].height);
		if (!success)
			return;

		faces[i].data = facebuffers[i].Elements();
	}

	_cubemap.Create(faces);
}

void Skybox::Render(RenderQueue& q) const
{
	{
		RenderEntry& e = q.CreateEntry(ERenderChannels::SKY);
		e.AddSetCubemap(_cubemap, 0);
		e.AddCommand(RCMDRenderMesh::INV_CUBE);
	}

	{
		RenderEntry& e = q.CreateEntry(ERenderChannels::SURFACE, -1);
		e.AddSetCubemap(_cubemap, 100);
	}
}
