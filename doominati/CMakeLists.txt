##-----------------------------------------------------------------------------
##
## Copyright (C) 2016-2017 Team Doominati
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
   dge-al-lib
   dge-code-lib
   dge-core-lib
   dge-fs-lib
   dge-game-lib
   dge-gl-lib
   ${DGE_MAIN_LIBRARIES})

if(DGE_INSTALL_EXE)
   install(TARGETS doominati RUNTIME DESTINATION bin)
endif()

## EOF

