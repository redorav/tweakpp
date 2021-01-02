#pragma once

#include "tppCommon.h"

#include <cstdint>

#if defined(TPP_SERVER)
#include <string>
#endif

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
			return currentValue;
		}

		// Need mutable for the server to modify value through the UI widgets
		mutable float currentValue = 0.0f;

		float minValue = 0.0f;

		float maxValue = 0.0f;

		float step = 0.0f;
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
}