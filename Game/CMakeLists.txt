##-----------------------------------------------------------------------------
##
## Copyright (C) 2016 Team Doominati
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
   Input.cpp
   Input.hpp
)

target_link_libraries(dge-game-lib dge-code-lib dge-gl-lib)

## EOF

