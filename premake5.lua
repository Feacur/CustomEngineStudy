workspace "CustomEngineStudy"
	-- toolset ("msc")
	architecture "x86_64"
	systemversion "latest"
	startproject "platform_windows"
	warnings "Extra"
	flags { "FatalWarnings", "NoMinimalRebuild", "MultiProcessorCompile" }
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
		symbols "On"
		optimize "Off"

	filter "configurations:Development"
		defines "DEVELOPMENT"
		staticruntime "Off"
		runtime "Release"
		symbols "On"
		optimize "On"

	filter "configurations:Shipping"
		-- either bundle CRT (vcruntime140.dll, etc.)
		-- or build with [staticruntime "On"]
		defines "SHIPPING"
		staticruntime "Off"
		runtime "Release"
		symbols "Off"
		optimize "On"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- project: demo game
project "demo_game"
	location "code"
	kind "SharedLib"
	language "C++"
	cdialect "C11"
	cppdialect "C++17"
	characterset ("MBCS")

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files {
		"code/%{prj.name}/**.h",
		"code/%{prj.name}/main.cpp",
	}

	includedirs {
		"code",
	}

	filter "system:windows"
		defines {
			"BUILD_DLL",
		}

		postbuildcommands {
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/platform_windows")
		}

-- project: platform windows
project "platform_windows"
	location "code"
	kind "ConsoleApp"
	language "C++"
	cdialect "C11"
	cppdialect "C++17"
	characterset ("MBCS")

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files {
		"code/%{prj.name}/**.h",
		"code/%{prj.name}/main.cpp",
	}

	includedirs {
		"code",
	}

	filter "system:windows"
		defines {
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
	location "code"
	kind "ConsoleApp"
	language "C++"
	cdialect "C11"
	cppdialect "C++17"
	characterset ("MBCS")

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files {
		"code/%{prj.name}/**.h",
		"code/%{prj.name}/main.cpp",
	}

	includedirs {
		"code",
	}

	filter "system:windows"
		defines {
			--
		}

-- project: demo c
project "demo_c"
	location "code"
	kind "ConsoleApp"
	language "C"
	cdialect "C11"
	cppdialect "C++17"
	characterset ("MBCS")

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files {
		"code/%{prj.name}/**.h",
		"code/%{prj.name}/main.c",
	}

	includedirs {
		"code",
	}

	filter "system:windows"
		defines {
			--
		}
