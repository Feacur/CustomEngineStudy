include "./premake5_customizations.lua"

workspace "CustomEngineStudy_v2"
	-- toolset ("msc")
	architecture "x86_64"
	systemversion "latest"
	startproject "sandbox"
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

	defines {
		--
	}

	filter "toolset:msc*"
		disablewarnings {
			"4201", -- nameless struct/union (suppress)
			"4189", -- local variable is initialized but not referenced (suppress)
			"4100", -- unreferenced formal parameter (suppress)
			"4530", -- C++ exception handling was used but -EHsc was not selected (suppress)
			"4702", -- unreachable code (suppress) -- fmt from spdlog causes it
			-- "4521", -- 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
			-- "4312", -- 'operation' : conversion from 'type1' to 'type2' of greater size
			-- "4127", -- conditional expression is constant
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
		symbols "Full" -- On, Full
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
enginename = "engine"

-- Include directories relative to the root folder (solution directory)
include_directories = {}
include_directories["engine"] = enginename .. "/src"

root_directory = os.getcwd()

-- project: engine
project "engine"
	location "engine"
	kind "StaticLib" -- or "SharedLib"
	language "C++"
	cdialect "C11"
	cppdialect "C++17"
	characterset ("ASCII") -- Default, Unicode, MBCS, ASCII

	targetdir (target_location .. "/%{prj.name}")
	objdir (intermediate_location .. "/%{prj.name}")

	-- pchheader "custom_pch.h"
	-- pchsource (enginename .. "/src/custom_pch.cpp")

	files {
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
	}

	includedirs {
		"%{include_directories.engine}",
	}

	filter "system:windows"
		defines {
			-- "CUSTOM_IS_DLL", -- if specified [kind "SharedLib"]
			-- "CUSTOM_SHARED", -- if specified [kind "SharedLib"]
		}

		links {
			-- "user32",
			-- "gdi32",
			-- "opengl32",
			-- "winmm",
			-- "kernel32",
		}

		postbuildcommands {
			-- ("{COPY} \"%{prj.location}assets\" \"../bin/" .. outputdir .. "/sandbox/assets\""),
			-- if specified [kind "SharedLib"]
			-- ("{COPY} \"%{cfg.buildtarget.relpath}\" \"../bin/" .. outputdir .. "/sandbox/\""),
		}

-- project: sandbox
project "sandbox"
	location "sandbox"
	kind "ConsoleApp"
	-- entrypoint "mainCRTStartup"
	language "C++"
	cdialect "C11"
	cppdialect "C++17"
	characterset ("ASCII") -- Default, Unicode, MBCS, ASCII

	targetdir (target_location .. "/%{prj.name}")
	objdir (intermediate_location .. "/%{prj.name}")

	files {
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
	}

	includedirs {
		"%{include_directories.engine}",
	}

	links {
		"engine",
	}

	postbuildcommands {
		-- ("{COPY} \"%{prj.location}assets\" \"%{cfg.buildtarget.directory}assets\"")
	}

	defines {
		-- "CUSTOM_SHARED", -- if specified [kind "SharedLib"] for the engine
	}
