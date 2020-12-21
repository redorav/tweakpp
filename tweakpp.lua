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
NetworkDirectory = SourceDirectory.."/platform"
UIDirectory = SourceDirectory.."/ui"

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
		UIDirectory.."/*.cpp"
	}

	includedirs
	{
		UIDirectory
	}

end

function AddImguiDependencies()

	files
	{
		"external/imgui/*.cpp",
		"external/imgui/backends/imgui_impl_win32.cpp",
		"external/imgui/backends/imgui_impl_dx11.cpp"
	}

	includedirs
	{
		"external/imgui"
	}

end

function AddGraphicsApiDependencies()

	links { "d3d11", "d3dcompiler", "dxgi" }

end

workspace "Tweak++ Server"
	configurations { "Debug", "Release" }
	location (Workspace.."/Server")
	defines { "_CRT_SECURE_NO_WARNINGS" }
	
project "Server"
	kind("consoleapp")
	language("C++")
	architecture("x64")
	AddNetworkDependencies()
	AddImguiDependencies()
	AddGraphicsApiDependencies()
	AddUIDependencies()
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
	AddNetworkDependencies()
	includedirs (SourceDirectory)
	files
	{
		SourceDirectory.."/*.cpp", SourceDirectory.."/*.h",
		SourceDirectory.."/client/*.cpp",
	}