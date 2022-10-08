#include "tppSaveData.h"

#include "pugixml.hpp"

tpp::ApplicationSettings tpp::SaveData::GlobalSettings;

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
