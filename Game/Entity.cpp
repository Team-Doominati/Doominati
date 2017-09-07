//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Reactive Thinkers.
//
//-----------------------------------------------------------------------------

#include "Game/Entity.hpp"

#include "Game/BlockMap.hpp"

#include "Code/Native.hpp"
#include "Code/Task.hpp"


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Game
{
   DGE_Game_ThinkerImplement(Entity);

   //
   // Entity::setTeam
   //
   void Entity::setTeam(Code::Word teamID)
   {
      teamLink.unlink();
      if((team = Team::Get(teamID)))
         teamLink.insert(&team->listEnt);
   }

   //
   // Entity::think
   //
   void Entity::think()
   {
      Super::think();

      // If no health remaining, die.
      if(health <= 0)
      {
         BlockMap::Root.unlink(this);
         unlink();
      }
   }
}


//----------------------------------------------------------------------------|
// Natives                                                                    |
//

namespace DGE::Game
{
   //
   // unsigned DGE_Entity_Create(unsigned ext)
   //
   DGE_Code_NativeDefn(DGE_Entity_Create)
   {
      std::size_t ext = argv[0];

      task->dataStk.push((new(ext) Entity)->id);
      return false;
   }
}

// EOF

