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

#include "Code/Convert.hpp"


//----------------------------------------------------------------------------|
// Macros                                                                     |
//

//
// DGE_Game_RenderThinker_GetMemberCases
//
#define DGE_Game_RenderThinker_GetMemberCases() \
   DGE_Game_PointThinker_GetMemberCases(); \
   case ThinkerMember::rsx:    return Code::HostToSAccum(rsx); \
   case ThinkerMember::rsy:    return Code::HostToSAccum(rsy); \
   case ThinkerMember::sprite: return sprite

//
// DGE_Game_RenderThinker_SetMemberCases
//
#define DGE_Game_RenderThinker_SetMemberCases() \
   DGE_Game_PointThinker_SetMemberCases(); \
   case ThinkerMember::rsx:    rsx = Code::SAccumToHost(val); break; \
   case ThinkerMember::rsy:    rsy = Code::SAccumToHost(val); break; \
   case ThinkerMember::sprite: sprite = val; break


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
      RenderThinker() : sprite{0}, rsx{0}, rsy{0} {}

      Code::Word sprite;

      std::float_t rsx, rsy;
   };
}

#endif//DGE__Game__RenderThinker_H__

