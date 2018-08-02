##-----------------------------------------------------------------------------
##
## Copyright (C) 2017 Team Doominati
##
## See COPYING for license information.
##
##-----------------------------------------------------------------------------
##
## CMake file for dge-sh.
##
##-----------------------------------------------------------------------------


##----------------------------------------------------------------------------|
## Targets                                                                    |
##

##
## dge-sh-lib
##
add_library(dge-sh-lib ${DGE_SHARED_DECL}
   Command.cpp
   Command.hpp
   Command/Misc.cpp
   Option.cpp
   Option.hpp
   Shell.cpp
   Shell.hpp
   StdIn.cpp
   StdIn.hpp
   Types.hpp
)

target_link_libraries(dge-sh-lib
   dge-code-lib
   dge-core-lib
   dge-defs-lib
   dge-fs-lib
   ${CMAKE_THREAD_LIBS_INIT}
   ${GDCC_LIBRARIES})

DGE_INSTALL_PART(sh FALSE TRUE)

## EOF

