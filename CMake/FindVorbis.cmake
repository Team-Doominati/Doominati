find_package(Ogg)

if(OGG_FOUND)
   find_path(Vorbis_INCLUDE_DIR vorbis/vorbisenc.h)
   find_library(Vorbis_LIBRARY NAMES vorbis)
   find_library(Vorbis_File_LIBRARY NAMES vorbisfile)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Vorbis REQUIRED_VARS OGG_LIBRARIES OGG_INCLUDE_DIRS Vorbis_LIBRARY Vorbis_File_LIBRARY Vorbis_INCLUDE_DIR)

if(Vorbis_FOUND)
   set(Vorbis_LIBRARIES ${Vorbis_LIBRARY} ${Vorbis_File_LIBRARY} ${OGG_LIBRARIES})
   set(Vorbis_INCLUDE_DIRS ${Vorbis_INCLUDE_DIR} ${OGG_INCLUDE_DIRS})
endif()

## EOF

