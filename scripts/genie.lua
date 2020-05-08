solution "UtilsCollectionTests"
	location				("../.project/".._ACTION)
	language				"C++"
	configurations			{ "Debug", "Release" }
	platforms				{ "x32", "x64" }
	objdir					("../.build/".._ACTION)

	project "JsonStthm"
		uuid				"abc3ee2c-722b-44ae-92b3-ed3f09900597"
		kind				"ConsoleApp"
		targetdir			"../Output/"

		files {
							"../JsonStthm/**.cpp",
							"../JsonStthm/**.h",

							"../../UtilsCollection/JsonStthm/**.cpp",
							"../../UtilsCollection/JsonStthm/**.h",

							-- https://github.com/vivkin/gason
							"../Externals/gason/src/gason.cpp",
							"../Externals/gason/src/gason.h",
							
							-- https://github.com/hjiang/jsonxx
							"../Externals/jsonxx/jsonxx.cc",
							"../Externals/jsonxx/jsonxx.h",
							
							--  https://github.com/sheredom/json.h
							"../Externals/json.h/json.h",
		}
		
		includedirs {
							"../JsonStthm/",
							"../../UtilsCollection/JsonStthm",
							"../Externals/gason/src",
							"../Externals/jsonxx/",
							"../Externals/json.h/"
		}

		configuration		"Debug"
			targetsuffix	"_d"
			flags			{ "Symbols" }
			
		configuration		"Release"
			targetsuffix	""
			flags			{ "Optimize" }