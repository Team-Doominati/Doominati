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

#ifndef DGE__Game__RenderThinker_H__
#define DGE__Game__RenderThinker_H__

#include "Game/PointThinker.hpp"


//----------------------------------------------------------------------------|
// Macros                                                                     |
//

//
// DGE_Game_RenderThinker_GetMemberCases
//
#define DGE_Game_RenderThinker_GetMemberCases() \
   DGE_Game_PointThinker_GetMemberCases()

//
// DGE_Game_RenderThinker_SetMemberCases
//
#define DGE_Game_RenderThinker_SetMemberCases() \
   DGE_Game_PointThinker_SetMemberCases()


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::Game
{
   //
   // RenderThinker
   //
   class RenderThinker : public PointThinker
   {
      DGE_Game_ThinkerPreamble(RenderThinker, PointThinker);

   public:
      RenderThinker() {}

      // TODO: Rendering information.
   };
}

#endif//DGE__Game__RenderThinker_H__

