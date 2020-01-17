project "demo_console"
	-- location "."
	kind "ConsoleApp"
	-- entrypoint "mainCRTStartup"
	language "C++"
	cdialect "C11"
	cppdialect "C++17"
	characterset ("ASCII") -- Default, Unicode, MBCS, ASCII

	demo_console_to_root = path.getrelative(os.getcwd(), root_directory)
	targetdir (demo_console_to_root .. "/" .. target_location .. "/%{prj.name}")
	objdir (demo_console_to_root .. "/" .. intermediate_location .. "/%{prj.name}")
	implibdir (demo_console_to_root .. "/" .. intermediate_location .. "/%{prj.name}")

	files {
		"%{prj.name}/**.h",
		"%{prj.name}/main.cpp",
	}

	includedirs {
		".",
	}

	filter {}
