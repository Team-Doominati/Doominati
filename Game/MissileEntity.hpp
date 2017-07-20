//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Missile/Projectile Entities.
//
//-----------------------------------------------------------------------------

#ifndef DGE__Game__MissileEntity_H__
#define DGE__Game__MissileEntity_H__

#include "../Game/Entity.hpp"

#include "Game/BlockMap.hpp"


//----------------------------------------------------------------------------|
// Macros                                                                     |
//

//
// DGE_Game_MissileEntity_GetMemberCases
//
#define DGE_Game_MissileEntity_GetMemberCases() \
   DGE_Game_Entity_GetMemberCases(); \
   case ObjectMember::damage: return damage; \
   case ObjectMember::owner:  return owner ? owner->id : 0

//
// DGE_Game_MissileEntity_SetMemberCases
//
#define DGE_Game_MissileEntity_SetMemberCases() \
   DGE_Game_Entity_SetMemberCases(); \
   case ObjectMember::damage: damage = val; break; \
   case ObjectMember::owner:  owner = Entity::Get(val); break


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::Game
{
   //
   // MissileEntity
   //
   class MissileEntity : public Entity
   {
      DGE_Game_ThinkerPreamble(MissileEntity, Entity);

   public:
      MissileEntity() {}

      virtual bool collideFrom(PhysicsThinker *th);
      virtual bool collideInto(PhysicsThinker *th);
      virtual bool collideInto(Sector *sec);

      Entity::Ptr  owner;
      std::int32_t damage;

   protected:
      virtual void think();
   };
}

#endif//DGE__Game__MissileEntity_H__

