project "custom_engine"
	-- location "."
	kind "StaticLib" -- or "SharedLib"
	language "C++"
	cdialect "C11"
	cppdialect "C++17"
	characterset ("ASCII") -- Default, Unicode, MBCS, ASCII

	engine_to_root = path.getrelative(os.getcwd(), root_directory)
	engine_target_location = (engine_to_root .. "/" .. target_location)
	engine_intermediate_location = (engine_to_root .. "/" .. intermediate_location)
	targetdir (engine_target_location .. "/%{prj.name}")
	objdir (engine_intermediate_location .. "/%{prj.name}")
	implibdir (engine_intermediate_location .. "/%{prj.name}")

	-- pchheader "custom_pch.h"
	-- pchsource ("src/custom_pch.cpp")
	-- defines "CUSTOM_PRECOMPILED_HEADER"

	files {
		"src/engine/**.h",
		"src/engine/**.cpp",
		-- "src/custom_pch.cpp",
	}

	includedirs {
		"%{engine_includes.custom_engine}",
		-- "%{engine_includes.imgui}",
		-- "%{engine_includes.lua}",
		"%{engine_includes.stb}",
	}

	links {
		-- "imgui",
		-- "lua",
		"stb",
	}

	postbuildcommands {
		("{COPY} \"%{prj.location}assets\" \"" .. engine_target_location .. "/sandbox/assets\""),
	}

	filter "system:windows"
		defines "WIN32_LEAN_AND_MEAN"

	filter "system:windows"
		links {
			-- "user32",
			-- "gdi32",
			"winmm",
		}
		files {
			"src/platform/windows/**.h",
			"src/platform/windows/**.cpp",
		}

	-- filter "system:windows or macosx or linux or bsd"
	-- 	defines "GLFW_INCLUDE_NONE"
	-- 	links "glfw"
	-- 	files {
	-- 		"src/platform/glfw/**.h",
	-- 		"src/platform/glfw/**.cpp",
	-- 	}
	-- 	includedirs "%{engine_includes.glfw}"

	filter "system:windows or macosx or linux or bsd"
		links "glad"
		files {
			"src/platform/opengl/**.h",
			"src/platform/opengl/**.cpp",
		}
		includedirs "%{engine_includes.glad}"

	filter "kind:SharedLib"
		postbuildcommands {
			("{COPY} \"%{cfg.buildtarget.relpath}\" \"" .. engine_target_location .. "/sandbox/\""),
		}

	filter {}
