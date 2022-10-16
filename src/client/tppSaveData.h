#pragma once

#include "tppClientCommon.h"

#include <string>

namespace tpp
{
	class VariableGroup;

	struct ApplicationSettings
	{
		tpp::SortOrder::T defaultSortOrder = SortOrder::None;
	};

	class SaveData
	{
	public:
		
		static void Initialize();

		static void SaveSettingsToFile(const ApplicationSettings& settings, const std::string& filename);

		static void LoadSettingsFromFile(ApplicationSettings& settings, const std::string& filename);

		static void SaveFavoritesToFile(const VariableGroup* favoritesGroup, const std::string& filename);

		static void LoadFavoritesFromFile(VariableGroup* favoritesGroup, const std::string& filename);

		static void SavePersistentVariablesToFile(const std::string& filename);

		static const std::string& GetTweakppDirectory();

		static const std::string& GetFavoritesDirectory();

		static const std::string& GetSettingsPath();

		// Directory here Tweakpp stores its persistent data
		static std::string TweakppDirectory;

		// Directory where favorite files live
		static std::string FavoritesDirectory;

		// Path to settings file
		static std::string SettingsPath;

		static ApplicationSettings GlobalSettings;
	};
};