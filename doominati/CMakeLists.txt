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

   ${CMAKE_SOURCE_DIR}/Core/Coord.hpp
   ${CMAKE_SOURCE_DIR}/Core/Search.hpp

   ${CMAKE_SOURCE_DIR}/FS/Dir/Directory.cpp
   ${CMAKE_SOURCE_DIR}/FS/Dir.cpp
   ${CMAKE_SOURCE_DIR}/FS/Dir.hpp

   ${CMAKE_SOURCE_DIR}/GL/Window.cpp
   ${CMAKE_SOURCE_DIR}/GL/Window.hpp

   ${CMAKE_SOURCE_DIR}/Game/Input.cpp
   ${CMAKE_SOURCE_DIR}/Game/Input.hpp
)

target_link_libraries(doominati gdcc-option-lib gdcc-core-lib SDL2 GL)

## EOF

