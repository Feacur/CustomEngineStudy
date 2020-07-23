project "platform_windows"
	-- location "."
	kind "ConsoleApp" -- WindowedApp
	-- entrypoint "mainCRTStartup"
	language "C++"
	cdialect "C11"
	cppdialect "C++17"
	characterset ("ASCII") -- Default, Unicode, MBCS, ASCII

	platform_windows_to_root = path.getrelative(os.getcwd(), root_directory)
	targetdir (platform_windows_to_root .. "/" .. target_location .. "/%{prj.name}")
	objdir (platform_windows_to_root .. "/" .. intermediate_location .. "/%{prj.name}")
	implibdir (platform_windows_to_root .. "/" .. intermediate_location .. "/%{prj.name}")

	files {
		"%{prj.name}/**.h",
		"%{prj.name}/main.cpp",
	}

	includedirs {
		".",
	}
	
	defines "WIN32_LEAN_AND_MEAN"
	
	links {
		"user32",
		"gdi32",
	}

	filter {}
