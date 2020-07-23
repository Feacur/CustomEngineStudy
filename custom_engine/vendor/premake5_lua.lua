project "lua"
	kind "StaticLib"
	language "C"
	cdialect "C89"
	cppdialect "C++11"
	characterset ("ASCII") -- Default, Unicode, MBCS, ASCII
	warnings "Default"
	removeflags { "FatalWarnings" }

	symbols "Off"
	optimize "On"
	
	lua_to_root = path.getrelative(os.getcwd(), root_directory)
	targetdir (lua_to_root .. "/" .. target_location .. "/vendor/%{prj.name}")
	objdir (lua_to_root .. "/" .. intermediate_location .. "/vendor/%{prj.name}")
	implibdir (lua_to_root .. "/" .. intermediate_location .. "/vendor/%{prj.name}")

	files {
		-- see ./lua/doc/readme.html
		"lua/src/lapi.c",
		"lua/src/lcode.c",
		"lua/src/lctype.c",
		"lua/src/ldebug.c",
		"lua/src/ldo.c",
		"lua/src/ldump.c",
		"lua/src/lfunc.c",
		"lua/src/lgc.c",
		"lua/src/llex.c",
		"lua/src/lmem.c",
		"lua/src/lobject.c",
		"lua/src/lopcodes.c",
		"lua/src/lparser.c",
		"lua/src/lstate.c",
		"lua/src/lstring.c",
		"lua/src/ltable.c",
		"lua/src/ltm.c",
		"lua/src/lundump.c",
		"lua/src/lvm.c",
		"lua/src/lzio.c",
		"lua/src/lauxlib.c",
		"lua/src/lbaselib.c",
		"lua/src/lcorolib.c",
		"lua/src/ldblib.c",
		"lua/src/liolib.c",
		"lua/src/lmathlib.c",
		"lua/src/loadlib.c",
		"lua/src/loslib.c",
		"lua/src/lstrlib.c",
		"lua/src/ltablib.c",
		"lua/src/lutf8lib.c",
		"lua/src/linit.c",
		"lua/src/*.h",
	}

	includedirs {
		"lua/src"
	}

	filter {}
