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
   ${CMAKE_SOURCE_DIR}/Code/MemPtr.hpp
   ${CMAKE_SOURCE_DIR}/Code/MemStr.cpp
   ${CMAKE_SOURCE_DIR}/Code/MemStr.hpp
   ${CMAKE_SOURCE_DIR}/Code/Memory.cpp
   ${CMAKE_SOURCE_DIR}/Code/Memory.hpp
   ${CMAKE_SOURCE_DIR}/Code/Native.cpp
   ${CMAKE_SOURCE_DIR}/Code/Native.hpp
   ${CMAKE_SOURCE_DIR}/Code/Native/File.cpp
   ${CMAKE_SOURCE_DIR}/Code/Native/Memory.cpp
   ${CMAKE_SOURCE_DIR}/Code/Native/Print.cpp
   ${CMAKE_SOURCE_DIR}/Code/Native/State.cpp
   ${CMAKE_SOURCE_DIR}/Code/OpCode.cpp
   ${CMAKE_SOURCE_DIR}/Code/OpCode.hpp
   ${CMAKE_SOURCE_DIR}/Code/OpList.hpp
   ${CMAKE_SOURCE_DIR}/Code/Process.cpp
   ${CMAKE_SOURCE_DIR}/Code/Process.hpp
   ${CMAKE_SOURCE_DIR}/Code/Program.hpp
   ${CMAKE_SOURCE_DIR}/Code/Task.cpp
   ${CMAKE_SOURCE_DIR}/Code/Task.hpp
   ${CMAKE_SOURCE_DIR}/Code/TaskExec.cpp
   ${CMAKE_SOURCE_DIR}/Code/Thread.cpp
   ${CMAKE_SOURCE_DIR}/Code/Thread.hpp
   ${CMAKE_SOURCE_DIR}/Code/Types.hpp

   ${CMAKE_SOURCE_DIR}/Core/BitStream.cpp
   ${CMAKE_SOURCE_DIR}/Core/BitStream.hpp
   ${CMAKE_SOURCE_DIR}/Core/HashMapFixed.hpp
   ${CMAKE_SOURCE_DIR}/Core/List.hpp
   ${CMAKE_SOURCE_DIR}/Core/Math.hpp
   ${CMAKE_SOURCE_DIR}/Core/NTS.cpp
   ${CMAKE_SOURCE_DIR}/Core/NTS.hpp
   ${CMAKE_SOURCE_DIR}/Core/Search.hpp
   ${CMAKE_SOURCE_DIR}/Core/Stack.hpp
   ${CMAKE_SOURCE_DIR}/Core/Store.hpp
   ${CMAKE_SOURCE_DIR}/Core/String.hpp
   ${CMAKE_SOURCE_DIR}/Core/Time.hpp
   ${CMAKE_SOURCE_DIR}/Core/Types.hpp
   ${CMAKE_SOURCE_DIR}/Core/Vector2.hpp
   ${CMAKE_SOURCE_DIR}/Core/Vector4.hpp
   ${CMAKE_SOURCE_DIR}/Core/Zip.cpp
   ${CMAKE_SOURCE_DIR}/Core/Zip.hpp

   ${CMAKE_SOURCE_DIR}/FS/Dir/Directory.cpp
   ${CMAKE_SOURCE_DIR}/FS/Dir.cpp
   ${CMAKE_SOURCE_DIR}/FS/Dir.hpp
   ${CMAKE_SOURCE_DIR}/FS/File.cpp
   ${CMAKE_SOURCE_DIR}/FS/File.hpp

   ${CMAKE_SOURCE_DIR}/GL/Color.cpp
   ${CMAKE_SOURCE_DIR}/GL/Color.hpp
   ${CMAKE_SOURCE_DIR}/GL/DynamicBuffer.cpp
   ${CMAKE_SOURCE_DIR}/GL/DynamicBuffer.hpp
   ${CMAKE_SOURCE_DIR}/GL/Particle.cpp
   ${CMAKE_SOURCE_DIR}/GL/Particle.hpp
   ${CMAKE_SOURCE_DIR}/GL/Shader.cpp
   ${CMAKE_SOURCE_DIR}/GL/Shader.hpp
   ${CMAKE_SOURCE_DIR}/GL/Texture.cpp
   ${CMAKE_SOURCE_DIR}/GL/Texture.hpp
   ${CMAKE_SOURCE_DIR}/GL/Texture/PAM.cpp
   ${CMAKE_SOURCE_DIR}/GL/Texture/PNG.cpp
   ${CMAKE_SOURCE_DIR}/GL/Window.cpp
   ${CMAKE_SOURCE_DIR}/GL/Window.hpp
   ${CMAKE_SOURCE_DIR}/GL/Window/Draw.cpp
   ${CMAKE_SOURCE_DIR}/GL/Window/PrivData.hpp
   ${CMAKE_SOURCE_DIR}/GL/Window/Texture.cpp
   ${CMAKE_SOURCE_DIR}/GL/gl_2_1.c
   ${CMAKE_SOURCE_DIR}/GL/gl_2_1.h

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
   ${SDL2_INCLUDE_DIRS}
   ${EIGEN3_INCLUDE_DIRS})

## EOF

