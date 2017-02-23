##-----------------------------------------------------------------------------
##
## Copyright (C) 2017 Team Doominati
##
## See COPYING for license information.
##
##-----------------------------------------------------------------------------
##
## Find package for SDL2.
##
## A platform independence layer library.
##
##-----------------------------------------------------------------------------

find_path(SDL2_INCLUDE_DIRS SDL.h
   HINTS ENV SDL2DIR
   PATH_SUFFIXES SDL2 include include/SDL2)

find_library(SDL2_LIBRARIES SDL2
   HINTS ENV SDL2DIR
   PATH_SUFFIXES lib)

find_library(SDL2_MAIN_LIBRARIES SDL2main
   HINTS ENV SDL2DIR
   PATH_SUFFIXES lib)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SDL2 REQUIRED_VARS SDL2_LIBRARIES SDL2_INCLUDE_DIRS)

set(SDL2_MAIN_FOUND NO)
if(SDL2_MAIN_LIBRARIES)
   set(SDL2_MAIN_FOUND YES)
endif()

## EOF

