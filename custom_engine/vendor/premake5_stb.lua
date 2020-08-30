project "stb"
	kind "StaticLib"
	language "C"
	cdialect "C89"
	characterset ("ASCII") -- Default, Unicode, MBCS, ASCII
	warnings "Default"
	removeflags { "FatalWarnings" }

	filter "configurations:not DebugLibs"
		symbols "Off"
		optimize "On"

	filter {}
	
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

	defines {
		"STB_IMAGE_IMPLEMENTATION",
		"STBI_NO_STDIO",
		"STBI_NO_JPEG",
		-- "STBI_NO_PNG",
		"STBI_NO_BMP",
		"STBI_NO_PSD",
		"STBI_NO_TGA",
		"STBI_NO_GIF",
		"STBI_NO_HDR",
		"STBI_NO_PIC",
		"STBI_NO_PNM",
	}
