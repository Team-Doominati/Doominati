##-----------------------------------------------------------------------------
##
## Copyright (C) 2016 Team Doominati
##
## See COPYING for license information.
##
##-----------------------------------------------------------------------------
##
## CMake file for doominati.
##
##-----------------------------------------------------------------------------


##----------------------------------------------------------------------------|
## Targets                                                                    |
##

##
## doominati
##

add_executable(doominati ${DGE_MAIN_ENTRY} main_doominati.cpp)

target_link_libraries(doominati
   dge-code-lib
   dge-core-lib
   dge-fs-lib
   dge-game-lib
   dge-gl-lib
   ${DGE_MAIN_LIBRARIES})

## EOF

