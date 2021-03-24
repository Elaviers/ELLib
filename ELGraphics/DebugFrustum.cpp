#include "DebugFrustum.hpp"
#include "RenderCommand.hpp"
#include "RenderQueue.hpp"
#include <ELMaths/Frustum.hpp>

void DebugFrustum::Render(RenderQueue& q) const
{
	RenderEntry& e = q.CreateEntry(ERenderChannels::EDITOR);
	e.AddSetColour(_colour);
	e.AddSetLineWidth(_lineWidth);

	e.AddLine(_near[0], _near[1]);
	e.AddLine(_near[1], _near[3]);
	e.AddLine(_near[3], _near[2]);
	e.AddLine(_near[2], _near[0]);

	e.AddLine(_near[0], _far[0]);
	e.AddLine(_near[1], _far[1]);
	e.AddLine(_near[2], _far[2]);
	e.AddLine(_near[3], _far[3]);

	e.AddLine(_far[0], _far[1]);
	e.AddLine(_far[1], _far[3]);
	e.AddLine(_far[3], _far[2]);
	e.AddLine(_far[2], _far[0]);
}

//static
DebugFrustum DebugFrustum::FromFrustum(const Frustum& inFrustum, float time, const Colour& colour, float lineWidth, float fadeTime)
{
	DebugFrustum frustum(time, colour, lineWidth, fadeTime);

	frustum._origin = inFrustum.position;
	frustum._near[0] = inFrustum.nearBtmLeft;
	frustum._near[1] = inFrustum.nearBtmRight;
	frustum._near[2] = inFrustum.nearTopLeft;
	frustum._near[3] = inFrustum.nearTopRight;
	frustum._far[0] = inFrustum.farBtmLeft;
	frustum._far[1] = inFrustum.farBtmRight;
	frustum._far[2] = inFrustum.farTopLeft;
	frustum._far[3] = inFrustum.farTopRight;

	return frustum;
}
