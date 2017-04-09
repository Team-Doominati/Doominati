//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Physics Thinkers.
//
//-----------------------------------------------------------------------------

#include "Game/PhysicsThinker.hpp"

#include "Code/Native.hpp"
#include "Code/Task.hpp"


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Game
{
   //
   // PhysicsThinker::think
   //
   void PhysicsThinker::think()
   {
      Super::think();

      // TODO: Physics.
   }
}


//----------------------------------------------------------------------------|
// Natives                                                                    |
//

namespace DGE::Game
{
   //
   // unsigned DGE_CreatePhysicsThinker(unsigned ext)
   //
   DGE_Code_NativeDefn(DGE_CreatePhysicsThinker)
   {
      std::size_t ext = argv[0];

      task->dataStk.push((new(ext) PhysicsThinker)->id);
      return false;
   }
}

// EOF

