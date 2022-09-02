#pragma once

namespace tpp
{
	namespace icons
	{
		// Standard Unicode Emojis

		// Used to signal disconnected status
		static constexpr const char* LargeRedCircle = "\xF0\x9F\x94\xB4"; // U+1F534

		// Used to signal connected status
		static constexpr const char* LargeGreenCircle = "\xF0\x9F\x9F\xA2"; // U+1F7E2

		static constexpr const char* LargeRedSquare = "\xF0\x9F\x9F\xA5"; // U+1F7E5

		// Used to mark a variable as persistent
		static constexpr const char* FloppyDisk = "\xF0\x9F\x92\xBE"; // U+1F4BE

		static constexpr const char* RedFlag = "\xF0\x9F\x9A\xA9"; // U+1F6A9

		static constexpr const char* YellowAlert = u8"\u26A0"; // U+26A0

		// FontAwesome custom emojis
		// https://fontawesome.com/v4/icon/file-code-o
		// Used as icon in the tree to signal variable group
		static constexpr const char* FileCode = u8"\uf1c9";

		// Used as icon for the address bar
		static constexpr const char* MagnifyingGlass = u8"\uF002";

		static constexpr const char* ElectricPlug = u8"\uF1E6";
	}
}