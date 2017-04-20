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
   BlockMap.cpp
   BlockMap.hpp
   Entity.cpp
   Entity.hpp
   Event.cpp
   Event.hpp
   Input.cpp
   Input.hpp
   Native.cpp
   Object.cpp
   Object.hpp
   PhysicsThinker.cpp
   PhysicsThinker.hpp
   PointThinker.cpp
   PointThinker.hpp
   RenderThinker.hpp
   Thinker.cpp
   Thinker.hpp
   ThinkerMemberList.hpp
   Types.hpp
)

target_link_libraries(dge-game-lib
   dge-code-lib
   dge-core-lib
   ${GDCC_LIBRARIES}
   ${SDL2_LIBRARIES})

## EOF

