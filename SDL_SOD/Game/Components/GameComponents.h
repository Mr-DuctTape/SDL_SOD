#pragma once
#include "../../Engine/ECS/Components.h"
#include <string>
#include <unordered_set>

struct EntityTag : Component
{
	std::string name;

	EntityTag* Clone() override
	{
		return new EntityTag(*this);
	}
	size_t Size() override
	{
		return sizeof(*this);
	}
};

