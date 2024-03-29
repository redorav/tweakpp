#pragma once

#include <string>

namespace tpp
{
	class Platform
	{
	public:

		static void Initialize();

		static const std::string& GetUserDirectory();
		
		static const std::string& GetTempDirectory();

		static const std::string& GetCurrentWorkingDirectory();

		static const std::string& GetExecutableDirectory();

		static const void CreateDirectory(const std::string& path);

		static const void CreateDirectories(const std::string& path);

	private:

		static void InitializePlatform();

		static std::string UserDirectory;

		static std::string TempDirectory;

		static std::string CurrentWorkingDirectory;

		static std::string ExecutableDirectory;
	};
};
