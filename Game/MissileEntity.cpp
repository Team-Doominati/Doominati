//-----------------------------------------------------------------------------
//
// Copyright (C) 2017-2019 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Missile/Projectile Entities.
//
//-----------------------------------------------------------------------------

#include "Game/MissileEntity.hpp"

#include "Code/Native.hpp"
#include "Code/Task.hpp"


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Game
{
   DGE_Code_NativeLoaderDefn(MissileEntity);
   DGE_Game_ThinkerImplement(MissileEntity);

   //
   // MissileEntity::collideFrom
   //
   bool MissileEntity::collideFrom(PhysicsThinker *)
   {
      // TODO: Flag to allow being collided into.
      return false;
   }

   //
   // MissileEntity::collideInto
   //
   bool MissileEntity::collideInto(PhysicsThinker *th)
   {
      // Don't collide if dead.
      if(health <= 0)
         return false;

      // Don't collide with owner.
      if(th == owner)
         return false;

      // Entity-specific handling.
      if(auto ent = dynamic_cast<Entity *>(th))
      {
         // Don't collide with same team.
         if(team && ent->team && *team == *ent->team)
            return false;

         ent->health -= damage;
      }

      health = 0;

      return true;
   }

   //
   // MissileEntity::collideInto
   //
   bool MissileEntity::collideInto(Sector *)
   {
      health = 0;

      return true;
   }

   //
   // MissileEntity::think
   //
   void MissileEntity::think()
   {
      Super::think();
   }
}


//----------------------------------------------------------------------------|
// Natives                                                                    |
//

namespace DGE::Game
{
   //
   // unsigned DGE_MissileEntity_Create(unsigned ext)
   //
   DGE_Code_NativeDefn(DGE_MissileEntity_Create)
   {
      task->dataStk.push(MissileEntity::Create(argv[0])->id);
      return false;
   }
}

// EOF

