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
   MemInfo.hpp
   MissileEntity.cpp
   MissileEntity.hpp
   Native.cpp
   Object.cpp
   Object.hpp
   ObjectMemberList.hpp
   PhysicsThinker.cpp
   PhysicsThinker.hpp
   PointThinker.cpp
   PointThinker.hpp
   RenderThinker.cpp
   RenderThinker.hpp
   Sector.cpp
   Sector.hpp
   Team.cpp
   Team.hpp
   Thinker.cpp
   Thinker.hpp
   Types.hpp
)

target_link_libraries(dge-game-lib
   dge-code-lib
   dge-core-lib
   ${GDCC_LIBRARIES}
   ${SDL2_LIBRARIES})

## EOF

