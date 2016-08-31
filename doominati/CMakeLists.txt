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
add_executable(doominati WIN32
   main_doominati.cpp

   ${CMAKE_SOURCE_DIR}/Code/Codedefs.cpp
   ${CMAKE_SOURCE_DIR}/Code/Codedefs.hpp
   ${CMAKE_SOURCE_DIR}/Code/Function.hpp
   ${CMAKE_SOURCE_DIR}/Code/OpCode.cpp
   ${CMAKE_SOURCE_DIR}/Code/OpCode.hpp
   ${CMAKE_SOURCE_DIR}/Code/OpList.hpp
   ${CMAKE_SOURCE_DIR}/Code/Program.hpp
   ${CMAKE_SOURCE_DIR}/Code/Types.hpp

   ${CMAKE_SOURCE_DIR}/Core/Coord.hpp
   ${CMAKE_SOURCE_DIR}/Core/NTS.cpp
   ${CMAKE_SOURCE_DIR}/Core/NTS.hpp
   ${CMAKE_SOURCE_DIR}/Core/Search.hpp
   ${CMAKE_SOURCE_DIR}/Core/String.hpp

   ${CMAKE_SOURCE_DIR}/FS/Dir/Directory.cpp
   ${CMAKE_SOURCE_DIR}/FS/Dir.cpp
   ${CMAKE_SOURCE_DIR}/FS/Dir.hpp
   ${CMAKE_SOURCE_DIR}/FS/File.cpp
   ${CMAKE_SOURCE_DIR}/FS/File.hpp

   ${CMAKE_SOURCE_DIR}/GL/Window.cpp
   ${CMAKE_SOURCE_DIR}/GL/Window.hpp

   ${CMAKE_SOURCE_DIR}/Game/Input.cpp
   ${CMAKE_SOURCE_DIR}/Game/Input.hpp
)

target_link_libraries(doominati
   ${GDCC_LIBRARIES}
   ${OPENGL_LIBRARIES}
   ${SDL2_LIBRARIES}
   ${SDL2_MAIN_LIBRARIES})

target_include_directories(doominati SYSTEM PRIVATE
   ${GDCC_INCLUDE_DIRS}
   ${OPENGL_INCLUDE_DIR}
   ${SDL2_INCLUDE_DIRS})

## EOF

