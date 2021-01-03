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
		Bool = 3,
		Color3 = 4,
		Color4 = 5,
		String = 6,
		Enum = 7,
		Function = 8,
		Invalid = 0xffffffff
	};

	class Float
	{
	public:

		Float(const char* path, float initialValue, float minValue, float maxValue, float step);

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
	public:

		UInt(const char* path, uint32_t initialValue, uint32_t minValue, uint32_t maxValue, uint32_t step);

		operator uint32_t()
		{
			return currentValue;
		}

		mutable uint32_t currentValue = 0u;

		uint32_t minValue = 0u;

		uint32_t maxValue = 0u;

		uint32_t step = 0u;
	};

	class Int
	{
	public:

		Int(const char* path, int32_t initialValue, int32_t minValue, int32_t maxValue, int32_t step);

		mutable int32_t currentValue = 0;

		uint32_t minValue = 0u;

		uint32_t maxValue = 0u;

		uint32_t step = 0u;
	};

	class Bool
	{
	public:

		Bool(const char* path, bool initialValue);

		mutable bool currentValue = 0;
	};

	class Color3
	{
	public:

		Color3(const char* path, float r, float g, float b);

		union
		{
			struct
			{
				float r;
				float g;
				float b;
			};

			float currentValue[3] = {};
		};
	};

	class Color4
	{
	public:

		Color4(const char* path, float r, float g, float b, float a);

		union
		{
			struct
			{
				float r;
				float g;
				float b;
				float a;
			};

			float currentValue[4] = {};
		};
	};

	class Vector3
	{
	public:

		Vector3(const char* path, float x, float y, float z);

		union
		{
			struct
			{
				float x;
				float y;
				float z;
			};

			float currentValue[3] = {};
		};
	};

	class Vector4
	{
	public:

		Vector4(const char* path, float x, float y, float z, float w);

		union
		{
			struct
			{
				float x;
				float y;
				float z;
				float w;
			};

			float currentValue[4] = {};
		};
	};

	// Can be any of the allowed types
	class Variable
	{
	public:

		Variable() {}

#if defined(TPP_SERVER)

		Variable(tpp::VariableType type, const std::string& path) : type(type), path(path)
		{
			if (path.size() > 0)
			{
				size_t lastSlash = path.find_last_of("/");
				size_t afterLastSlash = lastSlash + 1;

				if (lastSlash != std::string::npos)
				{
					groupPath = std::string(path.data(), lastSlash);
					name = std::string(path.data() + afterLastSlash, path.size() - afterLastSlash);
				}
			}
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

			tpp::UInt vdUInt;

			tpp::Int vdInt;

			tpp::Bool vdBool;
			
			tpp::Color3 vdColor3;
			
			tpp::Color4 vdColor4;

			tpp::Vector3 vdVector3;

			tpp::Vector4 vdVector4;
		};
	};

	template<int N>
	class dummy;

	//dummy<sizeof(Variable)>;
}