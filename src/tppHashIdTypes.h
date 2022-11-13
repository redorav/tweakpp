#pragma once

#include "tppHash.h"

namespace tpp
{
	struct VariableId : Hash
	{
		using Hash::Hash;
	};

	struct StringId : Hash
	{
		using Hash::Hash;
	};
};