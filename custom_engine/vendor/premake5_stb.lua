project "stb"
	kind "StaticLib"
	language "C"
	cdialect "C99"
	cppdialect "C++11"
	characterset ("ASCII") -- Default, Unicode, MBCS, ASCII
	warnings "Default"
	removeflags { "FatalWarnings" }

	symbols "Off"
	optimize "On"
	
	stb_to_root = path.getrelative(os.getcwd(), root_directory)
	targetdir (stb_to_root .. "/" .. target_location .. "/vendor/%{prj.name}")
	objdir (stb_to_root .. "/" .. intermediate_location .. "/vendor/%{prj.name}")
	implibdir (stb_to_root .. "/" .. intermediate_location .. "/vendor/%{prj.name}")

	files {
		"stb/stb_image.c",
	}

	includedirs {
		"stb"
	}
