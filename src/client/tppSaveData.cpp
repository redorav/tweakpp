#include "tppSaveData.h"

#include "tppClientVariableManager.h"
#include "platform/tppPlatform.h"

#include "pugixml.hpp"

tpp::ApplicationSettings tpp::SaveData::GlobalSettings;

std::string tpp::SaveData::TweakppDirectory;

std::string tpp::SaveData::FavoritesDirectory;

std::string tpp::SaveData::SettingsPath;

void tpp::SaveData::Initialize()
{
	TweakppDirectory = tpp::Platform::GetUserDirectory() + "Tweak++/";

	FavoritesDirectory = TweakppDirectory + "Favorites/";

	SettingsPath = TweakppDirectory + "Settings.xml";
}

void tpp::SaveData::SaveSettingsToFile(const ApplicationSettings& settings, const std::string& filename)
{
	pugi::xml_document doc;

	pugi::xml_node settingsNode = doc.append_child("Settings");

	pugi::xml_node sortOrderNode = settingsNode.append_child("VariableSortOrder");

	if (settings.defaultSortOrder == SortOrder::None)
	{
		sortOrderNode.text().set("None");
	}
	else if (settings.defaultSortOrder == SortOrder::Descending)
	{
		sortOrderNode.text().set("Descending");
	}
	else if (settings.defaultSortOrder == SortOrder::Ascending)
	{
		sortOrderNode.text().set("Ascending");
	}

	doc.save_file(filename.c_str());
}

void tpp::SaveData::LoadSettingsFromFile(ApplicationSettings& settings, const std::string& filename)
{
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(filename.c_str());

	if (result)
	{
		pugi::xml_node sortOrderNode = doc.child("Settings").child("VariableSortOrder");

		std::string sortOrderValue = sortOrderNode.first_child().value();

		if (sortOrderValue == "Descending")
		{
			settings.defaultSortOrder = SortOrder::Descending;
		}
		else if (sortOrderValue == "Ascending")
		{
			settings.defaultSortOrder = SortOrder::Ascending;
		}
		else
		{
			settings.defaultSortOrder = SortOrder::None;
		}
	}
}

void tpp::SaveData::SaveFavoritesToFile(const VariableGroup* favoritesGroup, const std::string& filename)
{
	pugi::xml_document doc;

	pugi::xml_node favoritesNode = doc.append_child("Favorites");

	for (const auto& variablePath : favoritesGroup->variableStrings)
	{
		favoritesNode.append_child("Variable").text().set(variablePath.c_str());
	}

	doc.save_file(filename.c_str());
}

void tpp::SaveData::LoadFavoritesFromFile(VariableGroup* favoritesGroup, const std::string& filename)
{
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(filename.c_str());

	pugi::xml_node favoritesNode = doc.child("Favorites");
	{
		for (pugi::xml_node child : favoritesNode.children("Variable"))
		{
			favoritesGroup->variableStrings.insert(child.text().as_string());
		}
	}
}

const std::string& tpp::SaveData::GetTweakppDirectory()
{
	return TweakppDirectory;
}

const std::string& tpp::SaveData::GetFavoritesDirectory()
{
	return FavoritesDirectory;
}

const std::string& tpp::SaveData::GetSettingsPath()
{
	return SettingsPath;
}
