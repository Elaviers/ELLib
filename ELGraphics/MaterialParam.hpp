#pragma once
#include <ELMaths/Vector2.hpp>

struct MaterialParam
{
	enum class EType
	{
		GRID_PARAM //Parameters for a MaterialGrid
	} type;

	union
	{
		struct
		{
			int row;
			int column;

			Vector2 uvScale;
		} gridData;

	};

	MaterialParam() {}
};
