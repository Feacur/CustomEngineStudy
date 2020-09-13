project "sandbox"
	kind "ConsoleApp" -- WindowedApp
	-- entrypoint "mainCRTStartup"
	language "C++"
	cdialect "C11"
	cppdialect "C++17"
	characterset ("ASCII") -- Default, Unicode, MBCS, ASCII

	-- filter {}

	sandbox_to_root = path.getrelative(os.getcwd(), root_directory)
	targetdir (sandbox_to_root .. "/" .. target_location .. "/%{prj.name}")
	objdir (sandbox_to_root .. "/" .. intermediate_location .. "/%{prj.name}")
	implibdir (sandbox_to_root .. "/" .. intermediate_location .. "/%{prj.name}")

	debugdir ("%{cfg.targetdir}")

	files {
		"src/**.h",
		"src/**.cpp",
	}

	includedirs {
		sandbox_to_root .. "/custom_engine/%{engine_includes.custom_engine}",
		-- sandbox_to_root .. "/custom_engine/%{engine_includes.imgui}",
		sandbox_to_root .. "/custom_engine/%{engine_includes.lua}",
	}

	defines {
		-- "CUSTOM_SYMBOLS_SHARE", -- if specified [kind "SharedLib"] for the engine
	}

	links {
		"custom_engine",
	}

	postbuildcommands {
		("xcopy /Q /Y /S /I /D \"%{prj.location}assets\" \"%{cfg.buildtarget.directory}assets\"")
	}
