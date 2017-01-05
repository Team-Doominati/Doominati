##-----------------------------------------------------------------------------
##
## Copyright (C) 2016-2017 Team Doominati
##
## See COPYING for license information.
##
##-----------------------------------------------------------------------------
##
## CMake file for dge-fs.
##
##-----------------------------------------------------------------------------


##----------------------------------------------------------------------------|
## Targets                                                                    |
##

##
## dge-fs-lib
##
add_library(dge-fs-lib STATIC
   Dir.cpp
   Dir.hpp
   Dir/Directory.cpp
   Dir/Pak.cpp
   File.cpp
   File.hpp
)

target_link_libraries(dge-fs-lib dge-core-lib)

## EOF

