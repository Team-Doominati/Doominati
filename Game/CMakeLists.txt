##-----------------------------------------------------------------------------
##
## Copyright (C) 2016-2017 Team Doominati
##
## See COPYING for license information.
##
##-----------------------------------------------------------------------------
##
## CMake file for dge-game.
##
##-----------------------------------------------------------------------------


##----------------------------------------------------------------------------|
## Targets                                                                    |
##

##
## dge-game-lib
##
add_library(dge-game-lib STATIC
   Event.cpp
   Event.hpp
   Input.cpp
   Input.hpp
   Native.cpp
)

target_link_libraries(dge-game-lib
   dge-code-lib
   dge-core-lib
   ${GDCC_LIBRARIES}
   ${SDL2_LIBRARIES})

## EOF

