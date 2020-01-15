include "./premake5_customizations.lua"

workspace "CustomEngineStudy_v1"
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

	defines {
		"_CRT_SECURE_NO_WARNINGS", -- @Note: might want to enable this only locally
	}

	filter "toolset:msc*"
		disablewarnings {
			"4201", -- nameless struct/union
			"4189", -- local variable is initialized but not referenced
			"4100", -- unreferenced formal parameter
			-- "4530", -- C++ exception handling was used but -EHsc was not selected
			-- "4702", -- unreachable code
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

	filter "kind:StaticLib"
		defines "CUSTOM_STATIC_LIBRARY"

	filter "kind:SharedLib"
		defines {
			"CUSTOM_SHARED_LIBRARY",
			"CUSTOM_SYMBOLS_SHARE",
		}

	filter "kind:ConsoleApp or WindowedApp"
		defines "CUSTOM_APPLICATION"

	filter "configurations:Debug"
		defines "CUSTOM_DEBUG"
		staticruntime "Off"
		runtime "Debug"
		symbols "On" -- On, Full
		optimize "Off" -- Off, Debug

	filter "configurations:Development"
		defines "CUSTOM_DEVELOPMENT"
		staticruntime "Off"
		runtime "Release"
		symbols "FastLink" -- On, FastLink
		optimize "On" -- On, Debug

	filter "configurations:Shipping"
		-- either bundle CRT (vcruntime140.dll, etc.)
		-- or build with [staticruntime "On"]
		defines "CUSTOM_SHIPPING"
		staticruntime "Off"
		runtime "Release"
		symbols "Off"
		optimize "On" -- On, Size, Speed, Full

outputdir = "v1/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
target_location = ("bin/" .. outputdir)
intermediate_location = ("bin-int/" .. outputdir)
enginename = "code"

engine_includes = {}
engine_includes["engine"] = enginename

root_directory = os.getcwd()

include "code/premake5_demo_game.lua"
include "code/premake5_platform_windows.lua"
include "code/premake5_demo_console.lua"
include "code/premake5_demo_c.lua"
