set(FREETYPE2_SEARCH_PATHS
   ~/Library/Frameworks
   /Library/Frameworks
   /usr/local
   /usr
   /sw
   /opt/local
   /opt/csw
   /opt
)

find_path(FREETYPE2_INCLUDE_DIRS
   NAMES ft2build.h
   PATH_SUFFIXES include/freetype2 include
   PATHS ${FREETYPE2_SEARCH_PATHS}
)

find_library(FREETYPE2_LIBRARIES
   NAMES freetype
   PATH_SUFFIXES lib64 lib
   PATHS ${FREETYPE2_SEARCH_PATHS}
)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(FREETYPE2 REQUIRED_VARS FREETYPE2_LIBRARIES FREETYPE2_INCLUDE_DIRS)

set(FREETYPE2_FOUND "NO")
if(FREETYPE2_LIBRARIES AND FREETYPE2_INCLUDE_DIRS)
   set(FREETYPE2_FOUND "YES")
endif()

## EOF

