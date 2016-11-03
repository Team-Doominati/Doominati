set(SDL2_SEARCH_PATHS
   ~/Library/Frameworks
   /Library/Frameworks
   /usr/local
   /usr
   /sw
   /opt/local
   /opt/csw
   /opt
)

find_path(SDL2_INCLUDE_DIRS SDL.h
   HINTS
   $ENV{SDL2DIR}
   PATH_SUFFIXES include/SDL2 include
   PATHS ${SDL2_SEARCH_PATHS}
)

find_library(SDL2_LIBRARIES
   NAMES SDL2
   HINTS
   $ENV{SDL2DIR}
   PATH_SUFFIXES lib64 lib
   PATHS ${SDL2_SEARCH_PATHS}
)

find_library(SDL2_MAIN_LIBRARIES
   NAMES SDL2main
   HINTS
   $ENV{SDL2DIR}
   PATH_SUFFIXES lib64 lib
   PATHS ${SDL2_SEARCH_PATHS}
)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(SDL2 REQUIRED_VARS SDL2_LIBRARIES SDL2_INCLUDE_DIRS)

set(SDL2_FOUND "NO")
set(SDL2_MAIN_FOUND "NO")

if(SDL2_LIBRARIES AND SDL2_INCLUDE_DIRS)
   set(SDL2_FOUND "YES")
endif()

if(SDL2_MAIN_LIBRARIES)
   set(SDL2_MAIN_FOUND "YES")
endif()

## EOF

