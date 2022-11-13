#pragma once

#if defined(TPP_CLIENT)

	#error Only include this file on the server

#endif

#include "tppHashIdTypes.h"

#include <unordered_map>

#include <string>

namespace tpp
{
	class VariableBase;
	enum class MessageType : uint8_t;

	struct VariableDescription
	{
		VariableDescription(tpp::VariableBase* variable, const std::string& path, const tpp::Hash& hash)
			: variable(variable)
			, path(path)
			, hash(hash)
		{}

		tpp::VariableBase* variable;
		std::string path;
		tpp::VariableId hash;
	};

	struct Operation
	{
		tpp::VariableId id = 0;
		std::string path;
		tpp::MessageType messageType = (tpp::MessageType)255;
	};

	class ServerVariableManager
	{
	public:

		void Register(const tpp::VariableDescription& variableDescription);

		void Unregister(const tpp::VariableDescription& variableDescription);

		const tpp::VariableDescription* Find(const tpp::VariableId& id) const;

		template<typename Fn>
		void ForEachVariable(Fn fn) const
		{
			for (const auto& it : m_variableHashmap)
			{
				fn(it.second.variable, it.second.path, it.first);
			}
		}

		bool HasPendingOperations() const;

		template<typename Fn>
		void ForEachPendingOperation(Fn fn) const
		{
			for (const Operation& operation : m_pendingOperations)
			{
				fn(operation);
			}
		}

		void ClearPendingOperations();

		void ResetVariableDescriptions();

	private:

		std::vector<Operation> m_pendingOperations;

		std::unordered_map<uint64_t, tpp::VariableDescription> m_variableHashmap;
	};

	ServerVariableManager* GetServerVariableManager();
}