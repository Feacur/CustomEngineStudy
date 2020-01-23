project "sandbox"
	-- location "."
	kind "ConsoleApp"
	-- entrypoint "mainCRTStartup"
	language "C++"
	cdialect "C11"
	cppdialect "C++17"
	characterset ("ASCII") -- Default, Unicode, MBCS, ASCII

	sandbox_to_root = path.getrelative(os.getcwd(), root_directory)
	targetdir (sandbox_to_root .. "/" .. target_location .. "/%{prj.name}")
	objdir (sandbox_to_root .. "/" .. intermediate_location .. "/%{prj.name}")
	implibdir (sandbox_to_root .. "/" .. intermediate_location .. "/%{prj.name}")

	files {
		"src/**.h",
		"src/**.cpp",
	}

	includedirs {
		sandbox_to_root .. "/custom_engine/%{engine_includes.custom_engine}",
	}

	defines {
		-- "CUSTOM_SYMBOLS_SHARE", -- if specified [kind "SharedLib"] for the engine
	}

	links {
		"custom_engine",
	}

	postbuildcommands {
		-- ("{COPY} \"%{prj.location}assets\" \"%{cfg.buildtarget.directory}assets\"")
	}

	filter {}
