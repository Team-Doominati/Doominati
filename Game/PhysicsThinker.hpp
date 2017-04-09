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

#ifndef DGE__Game__PhysicsThinker_H__
#define DGE__Game__PhysicsThinker_H__

#include "Game/RenderThinker.hpp"


//----------------------------------------------------------------------------|
// Macros                                                                     |
//

//
// DGE_Game_PhysicsThinker_GetMemberCases
//
#define DGE_Game_PhysicsThinker_GetMemberCases() \
   DGE_Game_RenderThinker_GetMemberCases(); \
   case ThinkerMember::mass: return mass; \
   case ThinkerMember::vx:   return vx.raw(); \
   case ThinkerMember::vy:   return vy.raw(); \
   case ThinkerMember::vz:   return vz.raw(); \
   case ThinkerMember::wx:   return wx.raw(); \
   case ThinkerMember::wy:   return wy.raw(); \
   case ThinkerMember::wz:   return wz.raw()

//
// DGE_Game_PhysicsThinker_SetMemberCases
//
#define DGE_Game_PhysicsThinker_SetMemberCases() \
   DGE_Game_RenderThinker_SetMemberCases(); \
   case ThinkerMember::mass: mass =val; break; \
   case ThinkerMember::vx:   vx.raw(val); break; \
   case ThinkerMember::vy:   vy.raw(val); break; \
   case ThinkerMember::vz:   vz.raw(val); break; \
   case ThinkerMember::wx:   wx.raw(val); break; \
   case ThinkerMember::wy:   wy.raw(val); break; \
   case ThinkerMember::wz:   wz.raw(val); break


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::Game
{
   //
   // PhysicsThinker
   //
   class PhysicsThinker : public RenderThinker
   {
      DGE_Game_ThinkerPreamble(PhysicsThinker, RenderThinker);

   public:
      PhysicsThinker() : mass{0}, vx{0}, vy{0}, vz{0}, wx{0}, wy{0}, wz{0} {}

      virtual void think();

      Code::Word mass;

      Coord vx;
      Coord vy;
      Coord vz;
      Coord wx;
      Coord wy;
      Coord wz;
   };
}

#endif//DGE__Game__PhysicsThinker_H__

