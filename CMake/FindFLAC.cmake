##-----------------------------------------------------------------------------
##
## Copyright (C) 2017 Team Doominati
##
## See COPYING for license information.
##
##-----------------------------------------------------------------------------
##
## Find package for libFLAC.
##
## An audio library by Xiph which handles FLAC and Ogg FLAC.
##
##-----------------------------------------------------------------------------

find_package(Ogg)

if(OGG_FOUND)
   find_path(FLAC_INCLUDE_DIR FLAC/all.h)
   find_library(FLAC_LIBRARY NAMES flac FLAC)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(FLAC REQUIRED_VARS FLAC_LIBRARY FLAC_INCLUDE_DIR OGG_LIBRARIES OGG_INCLUDE_DIRS)

if(FLAC_FOUND)
   set(FLAC_LIBRARIES ${FLAC_LIBRARY} ${OGG_LIBRARIES})
   set(FLAC_INCLUDE_DIRS ${FLAC_INCLUDE_DIR} ${OGG_INCLUDE_DIRS})
endif()

## EOF

