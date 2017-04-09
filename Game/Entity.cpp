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

#include "Code/Native.hpp"
#include "Code/Task.hpp"


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Game
{
   void Entity_NativeAdd() {}

   //
   // Entity::think
   //
   void Entity::think()
   {
      Super::think();
   }
}


//----------------------------------------------------------------------------|
// Natives                                                                    |
//

namespace DGE::Game
{
   //
   // unsigned DGE_CreateEntity(unsigned ext)
   //
   DGE_Code_NativeDefn(DGE_CreateEntity)
   {
      std::size_t ext = argv[0];

      task->dataStk.push((new(ext) Entity)->id);
      return false;
   }
}

// EOF

