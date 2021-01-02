#pragma once

#include "tppCommon.h"

#include <cstdint>
#include <unordered_map>
#include <memory>

namespace tpp
{
	enum class VariableType : uint32_t
	{
		Float = 0,
		UnsignedInteger = 1,
		Integer = 2,
		Color3 = 3,
		Color4 = 4,
		Bool = 5,
		Function = 6,
		Invalid = 0xffffffff
	};

	class Float
	{
	public:

		Float(const char* path, float initialValue, float minValue, float maxValue, float step);

		Float& operator = (float f) = delete;

		operator float()
		{
			return m_currentValue;
		}

		// Need mutable for the server to modify value through the UI widgets
		mutable float m_currentValue = 0.0f;

		float m_minValue = 0.0f;

		float m_maxValue = 0.0f;

		float m_step = 0.0f;
	};

	class UInt
	{
		uint32_t u;
	};

	class Int
	{
		int32_t i;
	};

	// Can be any of the allowed types
	class Variable
	{
	public:

		Variable() {}

#if defined(TPP_SERVER)

		Variable(tpp::VariableType type, const std::string& path) : type(type), path(path)
		{
			size_t lastSlash = path.find_last_of("/");
			size_t afterLastSlash = lastSlash + 1;

			groupPath = std::string(path.data(), lastSlash);
			name = std::string(path.data() + afterLastSlash, path.size() - afterLastSlash);
		}

		const std::string& GetGroupPath() const
		{
			return groupPath;
		}

		const std::string& GetName() const
		{
			return name;
		}

		std::string path;

		std::string groupPath;

		std::string name;

		tpp::VariableType type = tpp::VariableType::Invalid;

#endif

		uint32_t size = 0;

		void* memory = nullptr;

		union
		{
			tpp::Float vdFloat;

			tpp::Int vdInt;

			tpp::UInt vdUInt;
		};
	};

	// TODO Move to different file, which we only include if !TPP_SERVER
#if !defined(TPP_SERVER)

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

		const tpp::Variable Dummy;

		std::unordered_map<std::string, tpp::Variable> m_hashMap;
	};

	extern std::unique_ptr<ClientVariableManager> GlobalClientVariableManager;

	static void InitializeGlobalClientVariableManager()
	{
		if (!GlobalClientVariableManager)
		{
			GlobalClientVariableManager = std::unique_ptr<ClientVariableManager>(new ClientVariableManager());
		}
	}

#endif
}