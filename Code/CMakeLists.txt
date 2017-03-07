##-----------------------------------------------------------------------------
##
## Copyright (C) 2016-2017 Team Doominati
##
## See COPYING for license information.
##
##-----------------------------------------------------------------------------
##
## CMake file for dge-code.
##
##-----------------------------------------------------------------------------


##----------------------------------------------------------------------------|
## Targets                                                                    |
##

##
## dge-code-lib
##
add_library(dge-code-lib STATIC
   Callback.cpp
   Callback.hpp
   Codedefs.cpp
   Codedefs.hpp
   Convert.hpp
   Function.hpp
   Glyph.cpp
   Glyph.hpp
   Memory.cpp
   Memory.hpp
   MemPtr.hpp
   MemStr.cpp
   MemStr.hpp
   Native.cpp
   Native.hpp
   Native/Debug.cpp
   Native/File.cpp
   Native/Memory.cpp
   Native/Print.cpp
   Native/State.cpp
   OpCode.cpp
   OpCode.hpp
   OpList.hpp
   Process.cpp
   Process.hpp
   Program.cpp
   Program.hpp
   Task.cpp
   TaskExec.cpp
   Task.hpp
   Thread.cpp
   Thread.hpp
   Types.hpp
)

target_link_libraries(dge-code-lib
   dge-core-lib
   dge-fs-lib
   ${GDCC_LIBRARIES})

## EOF

