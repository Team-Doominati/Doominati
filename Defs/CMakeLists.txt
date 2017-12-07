##-----------------------------------------------------------------------------
##
## Copyright (C) 2017 Team Doominati
##
## See COPYING for license information.
##
##-----------------------------------------------------------------------------
##
## CMake file for dge-defs.
##
##-----------------------------------------------------------------------------


##----------------------------------------------------------------------------|
## Targets                                                                    |
##

##
## dge-defs-lib
##
add_library(dge-defs-lib ${DGE_SHARED_DECL}
   Gamedefs.cpp
   Gamedefs.hpp
   Types.hpp
)

target_link_libraries(dge-defs-lib
   dge-core-lib
   dge-fs-lib
   ${GDCC_LIBRARIES})

DGE_INSTALL_PART(defs FALSE TRUE)

## EOF

