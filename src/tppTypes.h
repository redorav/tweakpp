#pragma once

#include <cstdint>

#include <unordered_map>

#include <memory>

namespace tpp
{
	class Float
	{
	public:

		enum Properties
		{
			Size = sizeof(float)
		};

		Float(const char* path, float initialValue, float minValue, float maxValue, float step);

		Float& operator = (float f) = delete;

		operator float()
		{
			return m_currentValue;
		}

	private:

		float m_currentValue = 0.0f;

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

		Variable(const std::string& path) : path(path)
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