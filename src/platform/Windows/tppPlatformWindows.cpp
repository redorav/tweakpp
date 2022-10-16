#include "platform/tppPlatform.h"

#include <algorithm>

#include <windows.h>
#include <shlobj.h>

namespace tpp
{
	void ConvertWcharToUTF8AndNormalize(const wchar_t* wideString, std::string& utf8String)
	{
		// Get string length
		int stringLength = WideCharToMultiByte(CP_UTF8, 0, wideString, -1, NULL, 0, NULL, NULL);

		if (stringLength > 0)
		{
			// Resize incoming string
			utf8String.resize(stringLength - 1);

			// Copy contents
			WideCharToMultiByte(CP_UTF8, 0, wideString, -1, (char*)utf8String.data(), stringLength, NULL, NULL);

			// Replace backslashes with forward slashes
			std::replace(utf8String.begin(), utf8String.end(), '\\', '/');

			if (utf8String[utf8String.length() - 1] != '/')
			{
				utf8String += '/';
			}
		}
	}

	void Platform::InitializePlatform()
	{
		const uint32_t MaxPathSize = MAX_PATH + 1;

		WCHAR tempPath[MaxPathSize];

		DWORD cwdLength = GetCurrentDirectory(MaxPathSize, tempPath);

		if (cwdLength > 0)
		{
			ConvertWcharToUTF8AndNormalize(tempPath, CurrentWorkingDirectory);
		}

		// https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-gettemppatha
		// https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getlongpathnamea
		DWORD shortTempPathLength = GetTempPath(MaxPathSize, tempPath);
		if (shortTempPathLength > 0)
		{
			// From the docs: "You can use the same buffer you used for the lpszShortPath parameter"
			DWORD longTempPathLength = GetLongPathName(tempPath, tempPath, MaxPathSize);
			if (longTempPathLength > 0)
			{
				ConvertWcharToUTF8AndNormalize(tempPath, TempDirectory);
			}
		}

		PWSTR appDataPath = nullptr;

		// https://stackoverflow.com/questions/5920853/how-to-open-a-folder-in-appdata-with-c
		// 
		// https://docs.microsoft.com/en-us/windows/win32/api/shlobj_core/nf-shlobj_core-shgetknownfolderpath
		// https://docs.microsoft.com/en-us/windows/win32/shell/knownfolderid
		HRESULT hResult = SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &appDataPath);

		if (hResult == S_OK)
		{
			ConvertWcharToUTF8AndNormalize(appDataPath, UserDirectory);
		}
	}
};