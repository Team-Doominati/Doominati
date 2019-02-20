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
add_library(dge-fs-lib ${DGE_SHARED_DECL}
   Dir.cpp
   Dir.hpp
   Dir/Directory.cpp
   Dir/Pak.cpp
   Dir/Union.cpp
   Dir/Zip.cpp
   File.cpp
   File.hpp
   Path.cpp
   Path.hpp
   Types.hpp
)

target_link_libraries(dge-fs-lib
   dge-core-lib
   ${GDCC_LIBRARIES})

DGE_INSTALL_PART(fs FALSE TRUE)

## EOF

