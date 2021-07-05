#include "DebugString.hpp"
#include "Font.hpp"
#include "RenderQueue.hpp"

void DebugString::Render(RenderQueue& q) const
{
	_font->RenderString(q.CreateEntry(ERenderChannels::UNLIT), _string.begin(), _transform);
}
