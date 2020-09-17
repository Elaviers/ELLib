#include "Bounds.hpp"
#include "Maths.hpp"

void Bounds::RecalculateSphereBounds()
{
	radius = Maths::SquareRoot(Maths::Max((min - centre).Abs().LengthSquared(), (max - centre).Abs().LengthSquared()));
}
