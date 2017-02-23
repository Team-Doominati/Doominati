##-----------------------------------------------------------------------------
##
## Copyright (C) 2017 Team Doominati
##
## See COPYING for license information.
##
##-----------------------------------------------------------------------------
##
## Find package for libogg.
##
## Used by all of Xiph's audio libraries, libogg is an API for reading
## the Ogg bitstream format.
##
##-----------------------------------------------------------------------------

find_path(OGG_INCLUDE_DIRS ogg/ogg.h)
find_library(OGG_LIBRARIES ogg)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(Ogg REQUIRED_VARS OGG_LIBRARIES OGG_INCLUDE_DIRS)

## EOF

