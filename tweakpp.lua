Workspace = "workspace/".._ACTION

PlatformMSVC64	= "MSVC 64"
PlatformMSVC32	= "MSVC 32"
PlatformLLVM64	= "LLVM 64"
PlatformLLVM32	= "LLVM 32"

PlatformOSX64	= "OSX 64"

PlatformLinux64_GCC		= "Linux64 GCC"
PlatformLinux64_Clang	= "Linux64 Clang"

-- Directories
SourceDirectory = "src"
PlatformDirectory = SourceDirectory.."/platform"

function AddWinsockLibrary()
	links("Ws2_32")
end

function AddPlatformDependencies()

	AddWinsockLibrary()

	files
	{
		PlatformDirectory.."/POSIX/**.h", PlatformDirectory.."/POSIX/**.cpp"
	}

end

workspace "Tweak++ Server"
	configurations { "Debug", "Release" }	
	location (Workspace.."/Server")
	defines { "_CRT_SECURE_NO_WARNINGS" }
	
project "Server"
	kind("consoleapp")
	language("C++")
	architecture("x64")
	AddPlatformDependencies()
	includedirs (SourceDirectory)
	files
	{
		SourceDirectory.."/*.cpp", SourceDirectory.."/*.h",
		SourceDirectory.."/server/*.cpp",
	}
	
workspace "Tweak++ Client"
	configurations { "Debug", "Release" }	
	location (Workspace.."/Client")
	defines { "_CRT_SECURE_NO_WARNINGS" }
	
project "Client"
	kind("consoleapp")
	language("C++")
	architecture("x64")
	AddPlatformDependencies()
	includedirs (SourceDirectory)
	files
	{
		SourceDirectory.."/*.cpp", SourceDirectory.."/*.h",
		SourceDirectory.."/client/*.cpp",
	}