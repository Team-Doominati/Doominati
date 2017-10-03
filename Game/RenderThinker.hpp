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
   case ObjectMember::ca:     return Code::HostToULFract(ca); \
   case ObjectMember::cb:     return Code::HostToULFract(cb); \
   case ObjectMember::cg:     return Code::HostToULFract(cg); \
   case ObjectMember::cr:     return Code::HostToULFract(cr); \
   case ObjectMember::rsx:    return Code::HostToSAccum(rsx); \
   case ObjectMember::rsy:    return Code::HostToSAccum(rsy); \
   case ObjectMember::shader: return shader; \
   case ObjectMember::sprite: return sprite

//
// DGE_Game_RenderThinker_SetMemberCases
//
#define DGE_Game_RenderThinker_SetMemberCases() \
   DGE_Game_PointThinker_SetMemberCases(); \
   case ObjectMember::ca:     ca = Code::ULFractToHost(val); break; \
   case ObjectMember::cb:     cb = Code::ULFractToHost(val); break; \
   case ObjectMember::cg:     cg = Code::ULFractToHost(val); break; \
   case ObjectMember::cr:     cr = Code::ULFractToHost(val); break; \
   case ObjectMember::rsx:    rsx = Code::SAccumToHost(val); break; \
   case ObjectMember::rsy:    rsy = Code::SAccumToHost(val); break; \
   case ObjectMember::shader: shader = val; break; \
   case ObjectMember::sprite: sprite = val; break


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
      RenderThinker() : cr{1}, cg{1}, cb{1}, ca{1}, shader{0}, sprite{0},
         rsx{0}, rsy{0} {}

      std::float_t cr, cg, cb, ca;

      Code::Word shader, sprite;

      std::float_t rsx, rsy;
   };
}

#endif//DGE__Game__RenderThinker_H__

