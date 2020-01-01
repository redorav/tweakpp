Workspace = "workspace/".._ACTION

PlatformMSVC64	= "MSVC 64"
PlatformMSVC32	= "MSVC 32"
PlatformLLVM64	= "LLVM 64"
PlatformLLVM32	= "LLVM 32"

PlatformOSX64	= "OSX 64"

PlatformLinux64_GCC		= "Linux64 GCC"
PlatformLinux64_Clang	= "Linux64 Clang"

-- Directories
srcDir = "src"

workspace "tweak++"
	configurations { "Debug", "Release" }	
	location (Workspace)
	defines { "_CRT_SECURE_NO_WARNINGS" }
	
project "server"
	kind("consoleapp")
	language("C++")
	links("Ws2_32")
	files
	{
		srcDir.."/*.cpp", srcDir.."/*.h",
		srcDir.."/server/*.cpp",
	}
	
project "client"
	kind("consoleapp")
	language("C++")
	links("Ws2_32")
	files
	{
		srcDir.."/*.cpp", srcDir.."/*.h",
		srcDir.."/client/*.cpp",
	}