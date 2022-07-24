#pragma once

#if defined(TPP_SERVER)

	#error Only include this file on the client

#endif

#include "tppTypes.h"

#include <unordered_map>
#include <memory>

namespace tpp
{
	class ClientVariableManager
	{
	public:

		void Register(const std::string& path, const tpp::Variable& data)
		{
			m_hashMap.insert({ path, data });
		}

		const tpp::Variable& Find(const std::string& path) const
		{
			auto dataIterator = m_hashMap.find(path);

			if (dataIterator != m_hashMap.end())
			{
				return dataIterator->second;
			}
			else
			{
				return Dummy;
			}
		}

		template<typename Fn>
		void ForEachVariable(Fn fn) const
		{
			for (const auto& it : m_hashMap)
			{
				fn(it.first, it.second);
			}
		}

	private:

		const tpp::Variable Dummy = tpp::Variable(tpp::VariableType::Invalid);

		std::unordered_map<std::string, tpp::Variable> m_hashMap;
	};

	ClientVariableManager* GetClientVariableManager();
}