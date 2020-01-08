include "./premake5_customizations.lua"

workspace "CustomEngineStudy"
	-- toolset ("msc")
	architecture "x86_64"
	systemversion "latest"
	startproject "platform_windows"
	warnings "Extra"
	editandcontinue "Off"
	flags {
		"FatalWarnings",
		"NoMinimalRebuild",
		"MultiProcessorCompile",
		"NoIncrementalLink",
		-- "LinkTimeOptimization",
		-- "Maps",
		"NoPCH",
	}
	floatingpoint "Fast"
	floatingpointexceptions "off"
	exceptionhandling "Off"
	rtti "Off"

	configurations {
		"Debug",
		"Development",
		"Shipping",
	}

	filter "toolset:msc*"
		disablewarnings {
			"4201", -- nameless struct/union
			"4189", -- local variable is initialized but not referenced
			"4100", -- unreferenced formal parameter
			-- "4530", -- C++ exception handling was used but -EHsc was not selected
			-- "4521", -- 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
		}
		-- buildoptions {
		-- 	"-Bt+",
		-- 	"-d2cgsummary",
		-- 	"-d1reportTime",
		-- }
		-- linkoptions {
		-- 	"-time+",
		-- }

	filter "toolset:gcc*"
		disablewarnings { "unused-variable", "unused-parameter", "missing-field-initializers" }

	filter "toolset:clang*"
		disablewarnings { "unused-variable", "unused-parameter", "missing-field-initializers" }

	filter "configurations:Debug"
		defines "DEBUG"
		staticruntime "Off"
		runtime "Debug"
		symbols "On" -- On, Full
		optimize "Off" -- Off, Debug

	filter "configurations:Development"
		defines "DEVELOPMENT"
		staticruntime "Off"
		runtime "Release"
		symbols "FastLink" -- On, FastLink
		optimize "On" -- On, Debug

	filter "configurations:Shipping"
		-- either bundle CRT (vcruntime140.dll, etc.)
		-- or build with [staticruntime "On"]
		defines "SHIPPING"
		staticruntime "Off"
		runtime "Release"
		symbols "Off"
		optimize "On" -- On, Size, Speed, Full

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
target_location = ("bin/" .. outputdir)
intermediate_location = ("bin-int/" .. outputdir)
enginename = "code"

-- Include directories relative to the root folder (solution directory)
include_directories = {}
include_directories["engine"] = enginename

root_directory = os.getcwd()

-- project: demo game
project "demo_game"
	-- targetname "demo_game"
	location "code"
	kind "SharedLib"
	language "C++"
	cdialect "C11"
	cppdialect "C++17"
	characterset ("ASCII") -- Default, Unicode, MBCS, ASCII
	
	targetdir (target_location .. "/%{prj.name}")
	objdir (intermediate_location .. "/%{prj.name}")
	implibdir (intermediate_location .. "/%{prj.name}")

	files {
		"code/%{prj.name}/**.h",
		"code/%{prj.name}/main.cpp",
	}

	includedirs {
		"%{include_directories.engine}",
	}

	filter "system:windows"
		defines {
			"API_SHARED",
			"BUILD_DLL",
		}

		postbuildcommands {
			("{COPY} \"%{cfg.buildtarget.directory}%{prj.name}*\" \"../" .. target_location .. "/platform_windows\"")
		}

-- project: platform windows
project "platform_windows"
	-- targetname "demo_windows"
	location "code"
	kind "ConsoleApp"
	language "C++"
	cdialect "C11"
	cppdialect "C++17"
	characterset ("ASCII") -- Default, Unicode, MBCS, ASCII

	targetdir (target_location .. "/%{prj.name}")
	objdir (intermediate_location .. "/%{prj.name}")
	implibdir (intermediate_location .. "/%{prj.name}")

	files {
		"code/%{prj.name}/**.h",
		"code/%{prj.name}/main.cpp",
	}

	includedirs {
		"%{include_directories.engine}",
	}

	filter "system:windows"
		defines {
			"API_SHARED",
			"WIN32_LEAN_AND_MEAN",
		}

		links {
			"user32",
			"gdi32",
			"opengl32",
			-- "winmm",
			-- "kernel32",
		}

-- project: demo console
project "demo_console"
	-- targetname "demo_console"
	location "code"
	kind "ConsoleApp"
	language "C++"
	cdialect "C11"
	cppdialect "C++17"
	characterset ("ASCII") -- Default, Unicode, MBCS, ASCII

	targetdir (target_location .. "/%{prj.name}")
	objdir (intermediate_location .. "/%{prj.name}")
	implibdir (intermediate_location .. "/%{prj.name}")

	files {
		"code/%{prj.name}/**.h",
		"code/%{prj.name}/main.cpp",
	}

	includedirs {
		"%{include_directories.engine}",
	}

	filter "system:windows"
		defines {
			--
		}

-- project: demo c
project "demo_c"
	-- targetname "demo_c"
	location "code"
	kind "ConsoleApp"
	language "C"
	cdialect "C11"
	cppdialect "C++17"
	characterset ("ASCII") -- Default, Unicode, MBCS, ASCII

	targetdir (target_location .. "/%{prj.name}")
	objdir (intermediate_location .. "/%{prj.name}")
	implibdir (intermediate_location .. "/%{prj.name}")

	files {
		"code/%{prj.name}/**.h",
		"code/%{prj.name}/main.c",
	}

	includedirs {
		"%{include_directories.engine}",
	}

	filter "system:windows"
		defines {
			--
		}
