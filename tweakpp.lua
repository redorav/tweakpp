Workspace = "workspace/".._ACTION

PlatformMSVC64	= "MSVC 64"
PlatformMSVC32	= "MSVC 32"
PlatformLLVM64	= "LLVM 64"
PlatformLLVM32	= "LLVM 32"

PlatformOSX64	= "OSX 64"

PlatformLinux64_GCC		= "Linux64 GCC"
PlatformLinux64_Clang	= "Linux64 Clang"

-- Directories
SourceDirectory = "src/"
NetworkDirectory = SourceDirectory.."platform/"
UIDirectory = SourceDirectory.."ui/"
ClientDirectory = SourceDirectory.."client/"
ServerDirectory = SourceDirectory.."server/"
ServerExampleDirectory = SourceDirectory.."example/"

Font2HeaderProject = 'Font2Header'
GeneratedDirectory = '{%{wks.location}/generated/'
GeneratedFontHeader = GeneratedDirectory.."tppFontHeader.h"
GeneratedFontCpp = GeneratedDirectory.."tppFontHeader.cpp"

function AddWinsockLibrary()
	links("Ws2_32")
end

function AddNetworkDependencies()

	AddWinsockLibrary()

	files
	{
		NetworkDirectory.."/POSIX/**.h", NetworkDirectory.."/POSIX/**.cpp"
	}

end

function AddUIDependencies()

	files
	{
		UIDirectory.."/windows-dx11/**.cpp",
		UIDirectory.."/*.cpp", UIDirectory.."/*.h"
	}

	includedirs
	{
		UIDirectory
	}

end

function AddImguiDependencies()

	-- Disable obsolete stuff so we don't rely on it by accident
	defines { "IMGUI_DISABLE_OBSOLETE_FUNCTIONS", "IMGUI_ENABLE_FREETYPE", "IMGUI_USE_WCHAR32" }

	files
	{
		"external/imgui/*.cpp",
		"external/imgui/misc/freetype/*.cpp", "external/imgui/misc/freetype/*.h",
		"external/imgui/misc/cpp/*.cpp", "external/imgui/misc/cpp/*.h",
		"external/imgui/backends/imgui_impl_win32.cpp",
		"external/imgui/backends/imgui_impl_dx11.cpp"
	}

	includedirs
	{
		"external/imgui",
		"external/imgui/misc/freetype",
		"external/imgui/misc/cpp",
	}

end

function AddXxHashDependencies()

	files
	{
		"external/xxhash/xxhash.c",
		"external/xxhash/xxhash.h"
	}

	includedirs
	{
		"external/xxhash"
	}

end

function AddFreetypeDependencies()

	local freetype = "external/freetype/"
	local src = freetype.."src/"

	files
	{
		src.."base/ftbbox.c",
		src.."base/ftbdf.c",
		src.."base/ftbitmap.c",
		src.."base/ftcid.c",
		src.."base/ftfstype.c",
		src.."base/ftgasp.c",
		src.."base/ftglyph.c",
		src.."base/ftgxval.c",
		src.."base/ftmm.c",
		src.."base/ftotval.c",
		src.."base/ftpatent.c",
		src.."base/ftpfr.c",
		src.."base/ftstroke.c",
		src.."base/ftsynth.c",
		src.."base/fttype1.c",
		src.."base/ftwinfnt.c",
	
		src.."autofit/autofit.c",
		src.."bdf/bdf.c",
		src.."cff/cff.c",
		src.."dlg/dlgwrap.c",
		src.."base/ftbase.c",
		src.."cache/ftcache.c",
		src.."gzip/ftgzip.c",
		src.."base/ftinit.c",
		src.."lzw/ftlzw.c",
		src.."pcf/pcf.c",
		src.."pfr/pfr.c",
		
		src.."psaux/psaux.c",
		src.."pshinter/pshinter.c",
		src.."psnames/psmodule.c",
		src.."raster/raster.c",
		src.."sdf/sdf.c",
		src.."sfnt/sfnt.c",
		src.."smooth/smooth.c",
		src.."svg/svg.c",
		src.."truetype/truetype.c",
		src.."type1/type1.c",
		src.."cid/type1cid.c",
		src.."type42/type42.c",
		src.."winfonts/winfnt.c",
		
		freetype.."**.h",
		
		-- Windows-specific
		freetype.."builds/windows/ftsystem.c",
		freetype.."builds/windows/ftdebug.c",
	}

	includedirs
	{
		freetype,
		freetype.."include"
	}
	
	defines
	{
		"FT2_BUILD_LIBRARY"
	}

end

function AddGraphicsApiDependencies()

	links { "d3d11", "d3dcompiler", "dxgi" }

end

function AddPugiXmlDependencies()

	local pugixml = "external/pugixml/"

	includedirs
	{
		pugixml,
		pugixml.."src"
	}

	files
	{
		pugixml.."src/*.cpp",
		pugixml.."src/*.hpp"
	}

end

function AddCommonFlags()

	flags
	{
		"multiprocessorcompile", -- /MP
	}
	
	cppdialect("c++11")
	warnings('extra')
	
	filter { "configurations:Debug" }
		defines { "DEBUG" }
		symbols ("full")
		inlining("auto")
		optimize("debug")
		runtime('release')
		
	filter { "configurations:Release" }
		defines { "NDEBUG" }
		inlining("auto")
		optimize("speed")
		flags { 'linktimeoptimization' }
		runtime('release')

end

function CopyFileCommand(filePath, destinationPath)
	return '{copyfile} "'..filePath..'" "'..destinationPath..'"'
end

workspace("Tweak++ Client")

	configurations { "Debug", "Release" }
	location (Workspace.."/Client")
	defines { "_CRT_SECURE_NO_WARNINGS" }
	defines { "TPP_CLIENT" }
	
	AddCommonFlags()
	
project("Client")

	kind("consoleapp")
	language("C++")
	cppdialect("C++17")
	architecture("x64")
	
	dependson { "Font2Header" }

	AddNetworkDependencies()
	AddImguiDependencies()
	AddGraphicsApiDependencies()
	AddUIDependencies()
	AddXxHashDependencies()
	AddFreetypeDependencies()
	AddPugiXmlDependencies()
	
	includedirs
	{
		SourceDirectory,
		ClientDirectory,
		GeneratedDirectory
	}
	
	files
	{
		SourceDirectory.."*.cpp", SourceDirectory.."*.h",
		SourceDirectory.."platform/*.h",
		SourceDirectory.."platform/*.cpp",
		ClientDirectory.."*.cpp", ClientDirectory.."*.h",
		GeneratedFontHeader,
		GeneratedFontCpp
	}
	
	filter { "system:windows" }
		files
		{
			SourceDirectory.."platform/windows/*.h",
			SourceDirectory.."platform/windows/*.cpp"
		}
		
project(Font2HeaderProject)
	
	kind("consoleapp")
	language("C++")
	cppdialect("C++17")
	architecture("x64")
	
	optimize('speed')
	symbols('on')
	inlining('auto')
	flags { 'linktimeoptimization' }
	runtime('release')

	files
	{
		SourceDirectory.."font2header/*.cpp", SourceDirectory.."font2header/*.h",
	}
	
	local commandLine = '%{cfg.buildtarget.directory}/'..Font2HeaderProject..' '..path.getabsolute('fonts')..' '..'%{wks.location}/generated/'
	
	postbuildcommands
	{
		'{echo} '..commandLine,
		commandLine
	}
	
workspace("Tweak++ Server")

	configurations { "Debug", "Release" }
	location (Workspace.."/Server")

	AddCommonFlags()
	
project("Server")

	kind("consoleapp")
	language("C++")
	architecture("x64")
	AddNetworkDependencies()
	AddXxHashDependencies()
	includedirs (SourceDirectory)
	files
	{
		SourceDirectory.."*.cpp", SourceDirectory.."*.h",
		ServerDirectory.."*.cpp", ServerDirectory.."*.h",
		ServerExampleDirectory.."*.cpp",
	}