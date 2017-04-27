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

#include "Core/ListLinkVector.hpp"


//----------------------------------------------------------------------------|
// Macros                                                                     |
//

//
// DGE_Game_PhysicsThinker_GetMemberCases
//
#define DGE_Game_PhysicsThinker_GetMemberCases() \
   DGE_Game_RenderThinker_GetMemberCases(); \
   case ObjectMember::mass: return mass; \
   case ObjectMember::sx:   return sx.raw(); \
   case ObjectMember::sy:   return sy.raw(); \
   case ObjectMember::sz:   return sz.raw(); \
   case ObjectMember::vx:   return vx.raw(); \
   case ObjectMember::vy:   return vy.raw(); \
   case ObjectMember::vz:   return vz.raw()

//
// DGE_Game_PhysicsThinker_SetMemberCases
//
#define DGE_Game_PhysicsThinker_SetMemberCases() \
   DGE_Game_RenderThinker_SetMemberCases(); \
   case ObjectMember::mass: mass = val;  break; \
   case ObjectMember::sx:   sx.raw(val); break; \
   case ObjectMember::sy:   sy.raw(val); break; \
   case ObjectMember::sz:   sz.raw(val); break; \
   case ObjectMember::vx:   vx.raw(val); break; \
   case ObjectMember::vy:   vy.raw(val); break; \
   case ObjectMember::vz:   vz.raw(val); break


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
      PhysicsThinker() : blockLinks{this},
         mass{0}, vx{0}, vy{0}, vz{0}, sx{0}, sy{0}, sz{0} {}

      virtual void think();

      Core::ListLinkVector<PhysicsThinker> blockLinks;

      Code::Word mass;

      Coord vx;
      Coord vy;
      Coord vz;
      Coord sx;
      Coord sy;
      Coord sz;


      static bool Collide(PhysicsThinker *th, Coord &x, Coord &y, Coord &z);

      static bool TryMoveX(PhysicsThinker *th, Coord x);
      static bool TryMoveY(PhysicsThinker *th, Coord y);
      static bool TryMoveZ(PhysicsThinker *th, Coord z);
   };
}

#endif//DGE__Game__PhysicsThinker_H__

