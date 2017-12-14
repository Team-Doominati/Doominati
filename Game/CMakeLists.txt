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
add_library(dge-game-lib ${DGE_SHARED_DECL}
   BlockMap.cpp
   BlockMap.hpp
   Defs.cpp
   Defs.hpp
   Entity.cpp
   Entity.hpp
   Event.cpp
   Event.hpp
   Input.cpp
   Input.hpp
   Inventory.cpp
   Inventory.hpp
   MemInfo.hpp
   MissileEntity.cpp
   MissileEntity.hpp
   Object.cpp
   Object.hpp
   ObjectMemberList.hpp
   ParticleSys.cpp
   ParticleSys.hpp
   PhysicsThinker.cpp
   PhysicsThinker.hpp
   PointThinker.cpp
   PointThinker.hpp
   RenderThinker.cpp
   RenderThinker.hpp
   Sector.cpp
   Sector.hpp
   ScriptedEntity.cpp
   ScriptedEntity.hpp
   Team.cpp
   Team.hpp
   Thinker.cpp
   Thinker.hpp
   Types.hpp
)

target_link_libraries(dge-game-lib
   dge-code-lib
   dge-core-lib
   dge-defs-lib
   ${GDCC_LIBRARIES}
   ${SDL2_LIBRARIES})

DGE_INSTALL_PART(game FALSE TRUE)

## EOF

