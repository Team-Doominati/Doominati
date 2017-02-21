find_path(GDCC_INCLUDE_DIRS GDCC/Option/Base.hpp)
find_library(GDCC_LIBRARY_option gdcc-option-lib)
find_library(GDCC_LIBRARY_core gdcc-core-lib)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GDCC REQUIRED_VARS GDCC_LIBRARY_option GDCC_LIBRARY_core GDCC_INCLUDE_DIRS)

if(GDCC_FOUND)
   set(GDCC_LIBRARIES ${GDCC_LIBRARY_option} ${GDCC_LIBRARY_core})
endif()

## EOF

