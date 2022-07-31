#pragma once

#if defined(TPP_SERVER)

	#error Only include this file on the client

#endif

#include "tppTypes.h"

#include "tppHash.h"

#include <unordered_map>

namespace tpp
{
	struct VariableDescription
	{
		VariableDescription(const tpp::Variable& variable, const std::string& path)
		: variable(variable)
		, path(path)
		{}

		tpp::Variable variable;
		std::string path;
	};

	class ClientVariableManager
	{
	public:

		void Register(const tpp::VariableDescription& variableDescription);

		const tpp::Variable& Find(const tpp::Hash& hash) const;

		template<typename Fn>
		void ForEachVariable(Fn fn) const
		{
			for (const auto& it : m_variableHashmap)
			{
				fn(it.second.variable, it.second.path, it.first);
			}
		}

	private:

		const tpp::Variable Dummy = tpp::Variable(tpp::VariableType::Invalid);

		std::unordered_map<uint64_t, tpp::VariableDescription> m_variableHashmap;
	};

	ClientVariableManager* GetClientVariableManager();
}