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
   case ObjectMember::bvx:   return bvx.raw(); \
   case ObjectMember::bvy:   return bvy.raw(); \
   case ObjectMember::bvz:   return bvz.raw(); \
   case ObjectMember::grabx: return grabx.raw(); \
   case ObjectMember::graby: return graby.raw(); \
   case ObjectMember::grabz: return grabz.raw(); \
   case ObjectMember::mass:  return mass; \
   case ObjectMember::sx:    return sx.raw(); \
   case ObjectMember::sy:    return sy.raw(); \
   case ObjectMember::sz:    return sz.raw(); \
   case ObjectMember::vx:    return vx.raw(); \
   case ObjectMember::vy:    return vy.raw(); \
   case ObjectMember::vz:    return vz.raw()

//
// DGE_Game_PhysicsThinker_SetMemberCases
//
#define DGE_Game_PhysicsThinker_SetMemberCases() \
   DGE_Game_RenderThinker_SetMemberCases(); \
   case ObjectMember::bvx:   bvx.raw(val);   break; \
   case ObjectMember::bvy:   bvy.raw(val);   break; \
   case ObjectMember::bvz:   bvz.raw(val);   break; \
   case ObjectMember::grabx: grabx.raw(val); break; \
   case ObjectMember::graby: graby.raw(val); break; \
   case ObjectMember::grabz: grabz.raw(val); break; \
   case ObjectMember::mass:  mass  = val;    break; \
   case ObjectMember::sx:    sx.raw(val);    break; \
   case ObjectMember::sy:    sy.raw(val);    break; \
   case ObjectMember::sz:    sz.raw(val);    break; \
   case ObjectMember::vx:    vx.raw(val);    break; \
   case ObjectMember::vy:    vy.raw(val);    break; \
   case ObjectMember::vz:    vz.raw(val);    break


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::Game
{
   class Sector;

   //
   // PhysicsThinker
   //
   class PhysicsThinker : public RenderThinker
   {
      DGE_Game_ThinkerPreamble(PhysicsThinker, RenderThinker);

   public:
      PhysicsThinker() :
         blockLinks{this},
         mass{0},
         bvx{0}, bvy{0}, bvz{0},
         grabx{0}, graby{0}, grabz{0},
         sx{0}, sy{0}, sz{0},
         vx{0}, vy{0}, vz{0}
      {
      }

      // Called when another PhysicsThinker is colliding into this.
      // Returns true if this can be collided into.
      virtual bool collideFrom(PhysicsThinker *th);

      // Called when this is colliding with another PhysicsThinker.
      // Returns true if this can collide into.
      virtual bool collideInto(PhysicsThinker *th);

      // Called when this is colliding with a Sector.
      // Returns true if this can collide into.
      virtual bool collideInto(Sector *sec);

      Core::ListLinkVector<PhysicsThinker> blockLinks;

      Code::Word mass;

      Coord bvx;
      Coord bvy;
      Coord bvz;
      Coord grabx;
      Coord graby;
      Coord grabz;
      Coord sx;
      Coord sy;
      Coord sz;
      Coord vx;
      Coord vy;
      Coord vz;


      static bool Collide(PhysicsThinker *th, Coord &x, Coord &y, Coord &z);

      static bool TryMoveX(PhysicsThinker *th, Coord x);
      static bool TryMoveY(PhysicsThinker *th, Coord y);
      static bool TryMoveZ(PhysicsThinker *th, Coord z);

   protected:
      virtual void think();
   };
}

#endif//DGE__Game__PhysicsThinker_H__

