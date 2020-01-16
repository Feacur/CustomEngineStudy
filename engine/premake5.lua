project "engine"
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

	files {
		"src/engine/**.h",
		"src/engine/**.cpp",
	}

	includedirs {
		"%{engine_includes.engine}",
	}

	postbuildcommands {
		-- ("{COPY} \"%{prj.location}assets\" \"" .. engine_target_location .. "/sandbox/assets\""),
	}

	filter "system:windows"
		files {
			"src/platform/windows/**.h",
			"src/platform/windows/**.cpp",
			"src/platform/opengl/**.h",
			"src/platform/opengl/**.cpp",
		}

	filter "kind:SharedLib"
		postbuildcommands {
			("{COPY} \"%{cfg.buildtarget.relpath}\" \"" .. engine_target_location .. "/sandbox/\""),
		}
