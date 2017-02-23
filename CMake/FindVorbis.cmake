##-----------------------------------------------------------------------------
##
## Copyright (C) 2017 Team Doominati
##
## See COPYING for license information.
##
##-----------------------------------------------------------------------------
##
## Find package for libvorbis and libvorbisfile.
##
## An audio library by Xiph which handles Ogg Vorbis.
##
## The vorbisfile library is a set of convenience functions for decoding.
##
##-----------------------------------------------------------------------------

find_package(Ogg)

if(OGG_FOUND)
   find_path(VORBIS_INCLUDE_DIR vorbis/vorbisenc.h)
   find_library(VORBIS_LIBRARY NAMES vorbis)
   find_library(VORBIS_FILE_LIBRARY NAMES vorbisfile)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Vorbis REQUIRED_VARS OGG_LIBRARIES OGG_INCLUDE_DIRS VORBIS_LIBRARY VORBIS_FILE_LIBRARY VORBIS_INCLUDE_DIR)

if(VORBIS_FOUND)
   set(VORBIS_LIBRARIES ${VORBIS_LIBRARY} ${VORBIS_FILE_LIBRARY} ${OGG_LIBRARIES})
   set(VORBIS_INCLUDE_DIRS ${VORBIS_INCLUDE_DIR} ${OGG_INCLUDE_DIRS})
endif()

## EOF

