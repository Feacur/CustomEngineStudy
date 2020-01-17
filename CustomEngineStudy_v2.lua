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
		"_CRT_SECURE_NO_WARNINGS", -- @Note: might want to enable this only locally
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

	filter "system:windows"
		defines "WIN32_LEAN_AND_MEAN"

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
		symbols "Full" -- On, Full
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

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
target_location = ("bin/" .. outputdir)
intermediate_location = ("bin-int/" .. outputdir)
enginename = "engine"

engine_includes = {}
engine_includes["engine"] = "src"

root_directory = os.getcwd()

include "engine/premake5.lua"
include "sandbox/premake5.lua"
