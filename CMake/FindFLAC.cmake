find_package(Ogg)

if(OGG_FOUND)
   find_path(FLAC_INCLUDE_DIRS FLAC/all.h)
   find_library(FLAC_LIBRARIES NAMES flac FLAC)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(FLAC REQUIRED_VARS FLAC_LIBRARIES FLAC_INCLUDE_DIRS)

## EOF

