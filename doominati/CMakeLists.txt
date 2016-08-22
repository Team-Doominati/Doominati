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
add_executable(doominati
   main_doominati.cpp

   ../Core/Coord.hpp

   ../GL/Window.cpp
   ../GL/Window.hpp

   ../Game/Input.cpp
   ../Game/Input.hpp
)

target_link_libraries(doominati gdcc-option-lib gdcc-core-lib SDL2 GL)

## EOF

