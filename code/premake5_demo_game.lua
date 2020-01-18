project "demo_game"
	-- location "."
	kind "SharedLib"
	-- entrypoint "_DllMainCRTStartup"
	language "C++"
	cdialect "C11"
	cppdialect "C++17"
	characterset ("ASCII") -- Default, Unicode, MBCS, ASCII

	demo_game_to_root = path.getrelative(os.getcwd(), root_directory)
	demo_game_to_root_target_location = (demo_game_to_root .. "/" .. target_location)
	demo_game_to_root_intermediate_location = (demo_game_to_root .. "/" .. intermediate_location)
	targetdir (demo_game_to_root_target_location .. "/%{prj.name}")
	objdir (demo_game_to_root_intermediate_location .. "/%{prj.name}")
	implibdir (demo_game_to_root_intermediate_location .. "/%{prj.name}")

	files {
		"%{prj.name}/**.h",
		"%{prj.name}/main.cpp",
	}

	includedirs {
		".",
	}

	postbuildcommands {
		("{COPY} \"%{cfg.buildtarget.directory}%{prj.name}*\" \"" .. demo_game_to_root_target_location .. "/platform_windows\"")
	}

	filter {}
