include "./premake5_customizations.lua"

workspace "CustomEngineStudy"
	-- toolset ("msc")
	architecture "x86_64" -- x86, x86_64, ARM
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
		"DebugLibs",
		"Debug",
		"Development",
		"Shipping",
	}

	defines {
		"_CRT_SECURE_NO_WARNINGS", -- @Note: might want to enable this only locally
		-- "LUA_32BITS", -- @Todo: revisit this option later;
		--                         mind that compiling Lua produces warnings in this case,
		--                         but I've disabled fatal mode for the libs
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
		defines {
			"WIN32_LEAN_AND_MEAN",
			"NOMINMAX",
		}

	filter "kind:StaticLib"
		defines "CUSTOM_STATIC_LIBRARY"

	filter "kind:SharedLib"
		defines {
			"CUSTOM_SHARED_LIBRARY",
			"CUSTOM_SYMBOLS_SHARE",
		}

	filter "kind:ConsoleApp or WindowedApp"
		defines "CUSTOM_APPLICATION"

	filter "configurations:DebugLibs"
		defines "CUSTOM_DEBUG_LIBS"
		staticruntime "Off"
		runtime "Debug"
		symbols "Full" -- On, Full
		optimize "Off" -- Off, Debug

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

	filter {}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
target_location = ("bin/" .. outputdir)
intermediate_location = ("bin-int/" .. outputdir)
enginename = "custom_engine"

engine_includes = {}
engine_includes["custom_engine"] = "src"
engine_includes["glad"]  = "vendor/glad/include"
-- engine_includes["glfw"]  = "vendor/glfw/include"
-- engine_includes["imgui"] = "vendor/imgui"
engine_includes["lua"]   = "vendor/lua/src"
engine_includes["stb"]   = "vendor/stb"

root_directory = os.getcwd()

group "dependecies"
include "custom_engine/vendor/premake5_glad.lua"
-- include "custom_engine/vendor/premake5_glfw.lua"
-- include "custom_engine/vendor/premake5_imgui.lua"
include "custom_engine/vendor/premake5_lua.lua"
include "custom_engine/vendor/premake5_stb.lua"
group ""

include "custom_engine/premake5.lua"
include "sandbox/premake5.lua"
