#include <string>
#include <vector>
#include <algorithm>
#include <filesystem>

void SaveStringToPath(const std::filesystem::path& path, const std::string& data)
{
	std::filesystem::path parentPath = path.parent_path();

	// Ensure all parent directories are created before we try to open a new file
	std::filesystem::create_directories(parentPath);

	FILE* file = fopen(path.string().c_str(), "wb");
	if (file)
	{
		fwrite(data.c_str(), 1, data.length(), file);

		fclose(file);
	}
}

int main(int argc, char** argv)
{
	if (argc > 2)
	{
		const std::filesystem::path fontDirectory = argv[1];
		const std::filesystem::path destinationDirectory = argv[2];

		const std::string HeaderFilename = "tppFontHeader.h";
		const std::string CppFilename = "tppFontHeader.cpp";

		const std::filesystem::path HeaderPath = destinationDirectory / HeaderFilename;
		const std::filesystem::path CppPath = destinationDirectory / CppFilename;

		std::string headerData;
		headerData += "#pragma once\n\n#include <cstdint>\n\nnamespace tpp\n{\n";

		std::string cppData;
		cppData += "#include \"" + HeaderFilename + "\"\n\nnamespace tpp\n{\n";

		std::filesystem::directory_iterator iter(fontDirectory);
		for (const std::filesystem::directory_entry& directoryEntry : iter)
		{
			if (!directoryEntry.is_directory())
			{
				std::filesystem::path fontPath = directoryEntry.path();
				std::filesystem::path fontExtension = fontPath.extension();

				if (fontExtension == ".ttf" || fontExtension == ".otf")
				{
					FILE* fontFile = fopen(fontPath.string().c_str(), "rb");
					if (fontFile)
					{
						std::string FontIdentifier = fontPath.filename().replace_extension("").string();
						std::replace(FontIdentifier.begin(), FontIdentifier.end(), ' ', '_');
						std::replace(FontIdentifier.begin(), FontIdentifier.end(), '-', '_');

						FontIdentifier[0] = std::toupper(FontIdentifier[0]);

						std::vector<uint8_t> fileData;

						fseek(fontFile, 0, SEEK_END);
						long fileSize = ftell(fontFile);
						rewind(fontFile);

						headerData += "extern const uint32_t " + FontIdentifier + "SizeBytes;\n\n";

						cppData += "const uint32_t " + FontIdentifier + "SizeBytes = " + std::to_string(fileSize) + ";\n\n";

						headerData += "extern const uint64_t " + FontIdentifier + "[];\n\n";

						cppData += "const uint64_t " + FontIdentifier + "[] =\n{";

						fileData.resize(fileSize);

						fread(fileData.data(), 1, fileSize, fontFile);

						fclose(fontFile);

						for (size_t i = 0; i < fileData.size(); i += sizeof(uint64_t))
						{
							if (i % 48 == 0)
							{
								cppData += "\n\t";
							}

							uint64_t eightBytes = *(uint64_t*)(&fileData[i]);
							cppData += std::to_string(eightBytes) + ",";
						}

						cppData += "\n};";
					}
				}
			}
		}

		headerData += "};";
		cppData += "};";

		SaveStringToPath(HeaderPath, headerData);
		SaveStringToPath(CppPath, cppData);
	}
}