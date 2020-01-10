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
		"GLM_FORCE_DEPTH_ZERO_TO_ONE",
		"GLM_FORCE_LEFT_HANDED",
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
-- include_directories["Glad"]   = enginename .. "/vendor/Glad/include"
-- include_directories["GLFW"]   = enginename .. "/vendor/GLFW/include"
-- include_directories["glm"]    = enginename .. "/vendor/glm"
-- include_directories["imgui"]  = enginename .. "/vendor/imgui"
-- include_directories["lua"]    = enginename .. "/vendor/lua/src"
-- include_directories["spdlog"] = enginename .. "/vendor/spdlog/include"
-- include_directories["stb_image"] = enginename .. "/vendor/stb_image"

root_directory = os.getcwd()

group "Dependecies"
-- include "Engine/vendor/premake5_Glad.lua"
-- include "Engine/vendor/premake5_GLFW.lua"
-- include "Engine/vendor/premake5_imgui.lua"
-- include "Engine/vendor/premake5_lua.lua"
group ""

project "engine"
	location "engine"
	kind "StaticLib" -- or "SharedLib"
	language "C++"
	cdialect "C11"
	cppdialect "C++17"
	characterset ("ASCII") -- Default, Unicode, MBCS, ASCII

	targetdir (target_location .. "/%{prj.name}")
	objdir (intermediate_location .. "/%{prj.name}")

	-- pchheader "ges_pch.h"
	-- pchsource (enginename .. "/src/ges_pch.cpp")

	files {
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		-- "%{include_directories.spdlog}/spdlog/**.h",
		-- "%{include_directories.glm}/glm/**.hpp",
		-- "%{include_directories.glm}/glm/**.inl",
		-- "%{include_directories.stb_image}/**.h",
		-- "%{include_directories.stb_image}/**.cpp",
	}

	includedirs {
		"%{include_directories.engine}",
		-- "%{include_directories.Glad}",
		-- "%{include_directories.GLFW}",
		-- "%{include_directories.glm}",
		-- "%{include_directories.imgui}",
		-- "%{include_directories.lua}",
		-- "%{include_directories.spdlog}",
		-- "%{include_directories.stb_image}",
	}

	filter "system:windows"
		defines {
			-- "GES_BUILD_DLL", -- if specified [kind "SharedLib"]
			-- "GES_SHARED", -- if specified [kind "SharedLib"]
			-- "GLFW_INCLUDE_NONE",
			-- "_CRT_SECURE_NO_WARNINGS",
		}

		links {
			-- "GLFW",
			-- "Glad",
			-- "imgui",
			-- "lua",
			"opengl32.lib",
		}

		postbuildcommands {
			-- ("{COPY} \"%{prj.location}assets\" \"../bin/" .. outputdir .. "/sandbox/assets\""),
			-- if specified [kind "SharedLib"]
			-- ("{COPY} \"%{cfg.buildtarget.relpath}\" \"../bin/" .. outputdir .. "/sandbox/\""),
		}

project "sandbox"
	location "sandbox"
	kind "ConsoleApp"
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
		-- "%{include_directories.glm}",
		-- "%{include_directories.imgui}",
		-- "%{include_directories.lua}",
		-- "%{include_directories.spdlog}",
	}

	links {
		"engine",
	}

	postbuildcommands {
		-- ("{COPY} \"%{prj.location}assets\" \"%{cfg.buildtarget.directory}assets\"")
	}

	defines {
		-- "GES_SHARED", -- if specified [kind "SharedLib"] for the engine
	}

	-- filter "system:windows"
	-- 	defines {
	-- 		"_CRT_SECURE_NO_WARNINGS",
	-- 	}

	-- filter "toolset:msc*"
	-- 	defines {
	-- 		-- "IMGUI_API=__declspec(dllimport)", -- if specified [kind "SharedLib"] for the engine
	-- 	}
