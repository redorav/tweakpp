#pragma once

#include <cstdint>

#include <unordered_map>

#include <memory>

namespace tpp
{
	struct Data
	{
		void* memory = nullptr;
		uint32_t size = 0;
	};

	class VariableManager
	{
	public:

		void Register(const std::string& path, const Data& data)
		{
			GlobalVariableMap.insert({path, data});
		}

		Data Find(const std::string& path)
		{
			auto dataIterator = GlobalVariableMap.find(path);

			if (dataIterator != GlobalVariableMap.end())
			{
				return dataIterator->second;
			}
			else
			{
				return Data();
			}
		}

	private:

		std::unordered_map<std::string, Data> GlobalVariableMap;
	};

	extern std::unique_ptr<VariableManager> GlobalVariableManager;

	static void InitializeGlobalVariableManager()
	{
		if (!GlobalVariableManager)
		{
			GlobalVariableManager = std::unique_ptr<VariableManager>(new VariableManager());
		}
	}

	class Float
	{
	public:

		Float(const char* path, float initialValue, float minValue, float maxValue, float increment);

		Float& operator = (float f) = delete;

		operator float()
		{
			return m_f;
		}

	private:

		float m_f;
	};

	class Uint
	{
		uint32_t u;
	};

	class Int
	{
		int32_t i;
	};
}