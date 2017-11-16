//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Positioned Thinkers.
//
//-----------------------------------------------------------------------------

#include "Game/PointThinker.hpp"

#include "Code/Native.hpp"
#include "Code/Task.hpp"


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Game
{
   DGE_Game_ThinkerImplement(PointThinker);
}


//----------------------------------------------------------------------------|
// Natives                                                                    |
//

namespace DGE::Game
{
   //
   // unsigned DGE_PointThinker_Create(unsigned ext)
   //
   DGE_Code_NativeDefn(DGE_PointThinker_Create)
   {
      task->dataStk.push(PointThinker::Create(argv[0])->id);
      return false;
   }
}

// EOF

