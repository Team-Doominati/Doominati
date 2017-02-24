##-----------------------------------------------------------------------------
##
## Copyright (C) 2017 Team Doominati
##
## See COPYING for license information.
##
##-----------------------------------------------------------------------------
##
## Find package for libopus and libopusfile.
##
## An audio library by Xiph which handles Ogg Opus.
##
##-----------------------------------------------------------------------------

find_package(Ogg)

if(OGG_FOUND)
   find_path(OPUS_INCLUDE_DIR opus.h PATH_SUFFIXES opus include include/opus)
   find_library(OPUS_LIBRARY NAMES opus)
   find_library(OPUS_FILE_LIBRARY NAMES opusfile)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Opus REQUIRED_VARS OPUS_LIBRARY OPUS_FILE_LIBRARY OPUS_INCLUDE_DIR OGG_LIBRARIES OGG_INCLUDE_DIRS)

if(OPUS_FOUND)
   set(OPUS_LIBRARIES ${OPUS_LIBRARY} ${OPUS_FILE_LIBRARY} ${OGG_LIBRARIES})
   set(OPUS_INCLUDE_DIRS ${OPUS_INCLUDE_DIR} ${OGG_INCLUDE_DIRS})
endif()

## EOF

