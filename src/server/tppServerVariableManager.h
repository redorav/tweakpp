#pragma once

#if defined(TPP_CLIENT)

	#error Only include this file on the server

#endif

#include "tppTypes.h"

#include "tppHash.h"

#include <unordered_map>

namespace tpp
{
	struct VariableDescription
	{
		VariableDescription(tpp::VariableBase* variable, const std::string& path)
		: variable(variable)
		, path(path)
		{}

		tpp::VariableBase* variable;
		std::string path;
	};

	class ServerVariableManager
	{
	public:

		void Register(const tpp::VariableDescription& variableDescription);

		tpp::VariableBase* Find(const tpp::Hash& hash) const;

		template<typename Fn>
		void ForEachVariable(Fn fn) const
		{
			for (const auto& it : m_variableHashmap)
			{
				fn(it.second.variable, it.second.path, it.first);
			}
		}

	private:

		std::unordered_map<uint64_t, tpp::VariableDescription> m_variableHashmap;
	};

	ServerVariableManager* GetServerVariableManager();
}