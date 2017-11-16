//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Rendered Thinkers.
//
//-----------------------------------------------------------------------------

#include "Game/RenderThinker.hpp"

#include "Code/Native.hpp"
#include "Code/Task.hpp"


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Game
{
   DGE_Game_ThinkerImplement(RenderThinker);
}


//----------------------------------------------------------------------------|
// Natives                                                                    |
//

namespace DGE::Game
{
   //
   // unsigned DGE_RenderThinker_Create(unsigned ext)
   //
   DGE_Code_NativeDefn(DGE_RenderThinker_Create)
   {
      task->dataStk.push(RenderThinker::Create(argv[0])->id);
      return false;
   }
}

// EOF

