#include "tppPlatform.h"

#include <filesystem>

namespace tpp
{

	void Platform::Initialize()
	{
		InitializePlatform();
	}

	const std::string& Platform::GetUserDirectory()
	{
		return UserDirectory;
	}

	const std::string& Platform::GetTempDirectory()
	{
		return TempDirectory;
	}

	const std::string& Platform::GetCurrentWorkingDirectory()
	{
		return CurrentWorkingDirectory;
	}

	const std::string& Platform::GetExecutableDirectory()
	{
		return ExecutableDirectory;
	}

	const void Platform::CreateDirectory(const std::string& path)
	{
		std::filesystem::create_directory(path);
	}

	const void Platform::CreateDirectories(const std::string& path)
	{
		std::filesystem::create_directories(path);
	}

	std::string Platform::UserDirectory;

	std::string Platform::TempDirectory;

	std::string Platform::CurrentWorkingDirectory;

	std::string Platform::ExecutableDirectory;
};