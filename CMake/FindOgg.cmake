find_path(OGG_INCLUDE_DIRS ogg/ogg.h)
find_library(OGG_LIBRARIES ogg)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(OGG REQUIRED_VARS OGG_LIBRARIES OGG_INCLUDE_DIRS)

## EOF

