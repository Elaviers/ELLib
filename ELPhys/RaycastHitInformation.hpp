#pragma once
class FixedBody;
class PhysicsBody;

struct RaycastHitInformation
{
	const FixedBody* fBody;
	const PhysicsBody* pBody;
	float time;
};
