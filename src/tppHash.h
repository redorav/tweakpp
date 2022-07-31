#pragma once

#include "tppCommon.h"

#include "xxhash.h"

namespace tpp
{
	class Hash
	{
	public:

		Hash() {}

		Hash(uint64_t hash) : m_hash(hash) {}

		Hash(const char* data, uint64_t sizeBytes) : m_hash(XXH3_64bits(data, sizeBytes)) {}

		operator uint64_t() const
		{
			return m_hash;
		}

		uint64_t m_hash = 0;
	};
}