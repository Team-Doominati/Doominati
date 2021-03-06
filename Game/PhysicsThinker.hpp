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

#include "../Game/RenderThinker.hpp"

#include "../Core/ListLinkVector.hpp"


//----------------------------------------------------------------------------|
// Macros                                                                     |
//

//
// DGE_Game_PhysicsThinker_GetMemberCases
//
#define DGE_Game_PhysicsThinker_GetMemberCases() \
   DGE_Game_RenderThinker_GetMemberCases(); \
   case ObjectMember::bvx:      return bvx.raw(); \
   case ObjectMember::bvy:      return bvy.raw(); \
   case ObjectMember::bvz:      return bvz.raw(); \
   case ObjectMember::friction: return friction.raw(); \
   case ObjectMember::grabx:    return grabx.raw(); \
   case ObjectMember::graby:    return graby.raw(); \
   case ObjectMember::grabz:    return grabz.raw(); \
   case ObjectMember::gx:       return gx.raw(); \
   case ObjectMember::gy:       return gy.raw(); \
   case ObjectMember::gz:       return gz.raw(); \
   case ObjectMember::mass:     return mass.raw(); \
   case ObjectMember::sx:       return sx.raw(); \
   case ObjectMember::sy:       return sy.raw(); \
   case ObjectMember::sz:       return sz.raw(); \
   case ObjectMember::vx:       return vx.raw(); \
   case ObjectMember::vy:       return vy.raw(); \
   case ObjectMember::vz:       return vz.raw()

//
// DGE_Game_PhysicsThinker_SetMemberCases
//
#define DGE_Game_PhysicsThinker_SetMemberCases() \
   DGE_Game_RenderThinker_SetMemberCases(); \
   case ObjectMember::bvx:      bvx.raw(val);      break; \
   case ObjectMember::bvy:      bvy.raw(val);      break; \
   case ObjectMember::bvz:      bvz.raw(val);      break; \
   case ObjectMember::friction: friction.raw(val); break; \
   case ObjectMember::grabx:    grabx.raw(val);    break; \
   case ObjectMember::graby:    graby.raw(val);    break; \
   case ObjectMember::grabz:    grabz.raw(val);    break; \
   case ObjectMember::gx:       gx.raw(val);       break; \
   case ObjectMember::gy:       gy.raw(val);       break; \
   case ObjectMember::gz:       gz.raw(val);       break; \
   case ObjectMember::mass:     mass.raw(val);     break; \
   case ObjectMember::sx:       sx.raw(val);       break; \
   case ObjectMember::sy:       sy.raw(val);       break; \
   case ObjectMember::sz:       sz.raw(val);       break; \
   case ObjectMember::vx:       vx.raw(val);       break; \
   case ObjectMember::vy:       vy.raw(val);       break; \
   case ObjectMember::vz:       vz.raw(val);       break


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
      PhysicsThinker(Code::Word *emv_, std::size_t emc_) :
         RenderThinker{emv_, emc_},
         blockLinks{this},
         bvx{0}, bvy{0}, bvz{0},
         friction{0},
         grabx{0}, graby{0}, grabz{0},
         gx{0}, gy{0}, gz{0},
         mass{0},
         sx{0}, sy{0}, sz{0},
         vx{0}, vy{0}, vz{0}
      {
      }

      void applyFriction(Fract f);

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


      Fixed bvx, bvy, bvz; // Bounce Velociy (X/Y/Z)
      Fract friction;
      Fixed grabx, graby, grabz;
      Fixed gx, gy, gz; // Gravity (X/Y/Z)
      Fixed mass;
      Fixed sx, sy, sz; // Size (X/Y/Z)
      Fixed vx, vy, vz; // Velocity (X/Y/Z)


      static bool Collide(PhysicsThinker *th, Fixed &x, Fixed &y, Fixed &z, Fract &friction);

      static This *Create(Code::Word ext) {return CreateT<This>(ext);}

      static bool TryMoveX(PhysicsThinker *th, Fixed x, Fract &friction);
      static bool TryMoveY(PhysicsThinker *th, Fixed y, Fract &friction);
      static bool TryMoveZ(PhysicsThinker *th, Fixed z, Fract &friction);

   protected:
      virtual void think();
   };
}

#endif//DGE__Game__PhysicsThinker_H__

