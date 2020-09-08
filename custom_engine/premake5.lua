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
		"%{engine_includes.lua}",
		"%{engine_includes.stb}",
	}

	defines {
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

	links {
		-- "imgui",
		"lua",
		"stb",
	}

	postbuildcommands {
		("xcopy /Q /Y /S /I /D \"%{prj.location}assets\" \"" .. engine_target_location .. "/sandbox/assets\""),
	}

	filter "system:windows"
		defines {
			"WIN32_LEAN_AND_MEAN",
			"NOMINMAX",
			-- "NOGDICAPMASKS",
			-- "NOVIRTUALKEYCODES",
			-- "NOWINMESSAGES",
			-- "NOWINSTYLES",
			-- "NOSYSMETRICS",
			-- "NOMENUS",
			-- "NOICONS",
			-- "NOKEYSTATES",
			-- "NOSYSCOMMANDS",
			-- "NORASTEROPS",
			-- "NOSHOWWINDOW",
			-- "OEMRESOURCE",
			-- "NOATOM",
			-- "NOCLIPBOARD",
			-- "NOCOLOR",
			-- "NOCTLMGR",
			-- "NODRAWTEXT",
			-- "NOGDI",
			-- "NOKERNEL",
			-- "NOUSER",
			-- "NONLS",
			-- "NOMB",
			-- "NOMEMMGR",
			-- "NOMETAFILE",
			-- "NOMINMAX",
			-- "NOMSG",
			"NOOPENFILE",
			-- "NOSCROLL",
			-- "NOSERVICE",
			-- "NOSOUND",
			-- "NOTEXTMETRIC",
			-- "NOWH",
			-- "NOWINOFFSETS",
			-- "NOCOMM",
			-- "NOKANJI",
			-- "NOHELP",
			-- "NOPROFILER",
			-- "NODEFERWINDOWPOS",
			-- "NOMCX",
		}

	-- @Todo: build setup issue? clang issue? CAN link
	filter { "system:windows", "toolset:not msc-ClangCL" }
		links {
			-- "user32",
			-- "gdi32",
			"winmm",
		}

	-- @Todo: build setup issue? clang issue? UNABLE to link
	filter { "system:windows", "toolset:not msc-ClangCL" }
		links {
			-- "user32",
			-- "gdi32",
			-- "winmm",
		}

	filter "system:windows"
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

	-- @Note: testing custom xcopy calls instead
	-- filter "kind:SharedLib"
	-- 	postbuildcommands {
	-- 		("{COPY} \"%{cfg.buildtarget.relpath}\" \"" .. engine_target_location .. "/sandbox/\""),
	-- 	}

	filter {}
