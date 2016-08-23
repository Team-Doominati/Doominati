SET(SDL2_SEARCH_PATHS
	~/Library/Frameworks
	/Library/Frameworks
	/usr/local
	/usr
	/sw
	/opt/local
	/opt/csw
	/opt
)

FIND_PATH(SDL2_INCLUDE_DIRS SDL.h
	HINTS
	$ENV{SDL2DIR}
	PATH_SUFFIXES include/SDL2 include
	PATHS ${SDL2_SEARCH_PATHS}
)

FIND_LIBRARY(SDL2_LIBRARIES
	NAMES SDL2
	HINTS
	$ENV{SDL2DIR}
	PATH_SUFFIXES lib64 lib
	PATHS ${SDL2_SEARCH_PATHS}
)

FIND_LIBRARY(SDL2_MAIN_LIBRARIES
	NAMES SDL2main
	HINTS
	$ENV{SDL2DIR}
	PATH_SUFFIXES lib64 lib
	PATHS ${SDL2_SEARCH_PATHS}
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(SDL2 REQUIRED_VARS SDL2_MAIN_LIBRARIES SDL2_LIBRARIES SDL2_INCLUDE_DIRS)

SET(SDL2_FOUND "NO")
IF(SDL2_LIBRARIES AND SDL2_MAIN_LIBRARIES AND SDL2_INCLUDE_DIRS)
	SET(SDL2_FOUND "YES")
ENDIF()

## EOF

