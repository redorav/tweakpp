#pragma once

#include "tppClientCommon.h"

#include <string>

namespace tpp
{
	struct ApplicationSettings
	{
		tpp::SortOrder::T defaultSortOrder = SortOrder::None;
	};

	class SaveData
	{
	public:
		
		static void SaveSettingsToFile(const ApplicationSettings& settings, const std::string& filename);

		static void LoadSettingsFromFile(ApplicationSettings& settings, const std::string& filename);

		static void SaveFavoritesToFile(const std::string& filename);

		static void SavePersistentVariablesToFile(const std::string& filename);

		static ApplicationSettings GlobalSettings;
	};
};