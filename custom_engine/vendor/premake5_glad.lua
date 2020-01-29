project "glad"
	kind "StaticLib"
	language "C"
	cdialect "C99"
	cppdialect "C++11"
	characterset ("ASCII") -- Default, Unicode, MBCS, ASCII
	warnings "Default"
	removeflags { "FatalWarnings" }

	filter "toolset:msc*"
		disablewarnings {
			"4005", -- 'identifier' : macro redefinition (suppress) -- might ignore it as well, though
		}

	symbols "Off"
	optimize "On"
	
	glad_to_root = path.getrelative(os.getcwd(), root_directory)
	targetdir (glad_to_root .. "/" .. target_location .. "/vendor/%{prj.name}")
	objdir (glad_to_root .. "/" .. intermediate_location .. "/vendor/%{prj.name}")
	implibdir (glad_to_root .. "/" .. intermediate_location .. "/vendor/%{prj.name}")

	files {
		"glad/include/KHR/khrplatform.h",
		"glad/include/glad/glad.h",
		"glad/src/glad.c",
	}

	includedirs {
		"glad/include"
	}
