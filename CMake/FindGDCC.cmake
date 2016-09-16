set(GDCC_SEARCH_PATHS
   ~/Library/Frameworks
   /Library/Frameworks
   /usr/local
   /usr
   /sw
   /opt/local
   /opt/csw
   /opt
)

find_path(GDCC_INCLUDE_DIRS
   NAMES GDCC/Option/Base.hpp
   PATH_SUFFIXES include
   PATHS ${GDCC_SEARCH_PATHS}
)

find_library(GDCC_LIBRARY_option
   NAMES gdcc-option-lib
   PATH_SUFFIXES lib64 lib
   PATHS ${GDCC_SEARCH_PATHS}
)

find_library(GDCC_LIBRARY_core
   NAMES gdcc-core-lib
   PATH_SUFFIXES lib64 lib
   PATHS ${GDCC_SEARCH_PATHS}
)

set(GDCC_LIBRARIES ${GDCC_LIBRARY_option} ${GDCC_LIBRARY_core})

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GDCC REQUIRED_VARS GDCC_LIBRARIES GDCC_INCLUDE_DIRS)

set(GDCC_FOUND "NO")
if(GDCC_LIBRARIES AND GDCC_INCLUDE_DIRS)
   set(GDCC_FOUND "YES")
endif()

## EOF

