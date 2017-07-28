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

#include "Game/PhysicsThinker.hpp"

#include "Game/BlockMap.hpp"

#include "Code/Native.hpp"
#include "Code/Task.hpp"


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Game
{
   //
   // PhysicsThinker::collideFrom
   //
   bool PhysicsThinker::collideFrom(PhysicsThinker *)
   {
      return true;
   }

   //
   // PhysicsThinker::collideInto
   //
   bool PhysicsThinker::collideInto(PhysicsThinker *)
   {
      return true;
   }

   //
   // PhysicsThinker::collideInto
   //
   bool PhysicsThinker::collideInto(Sector *)
   {
      return true;
   }

   //
   // PhysicsThinker::think
   //
   void PhysicsThinker::think()
   {
      Super::think();

      // TODO: Gravity.

      // Apply velocity.
      BlockMap::Root.unlink(this);

      auto avx = std::abs(vx);
      auto avy = std::abs(vy);
      auto avz = std::abs(vz);
      if(avx >= sx || avy >= sy || avz >= sz)
      {
         Coord stepx, stepy, stepz;
         Coord::I steps;

         // Never move more than one width at a time.
         steps = static_cast<Coord::I>(std::ceil(std::max({avx / sx, avy / sy, avz / sz}))) + 1;
         stepx = vx / steps;
         stepy = vy / steps;
         stepz = vz / steps;

         while(steps--)
         {
            if(stepx && !TryMoveX(this, x + stepx)) stepx = vx = 0;
            if(stepy && !TryMoveY(this, y + stepy)) stepy = vy = 0;
            if(stepz && !TryMoveZ(this, z + stepz)) stepz = vz = 0;
         }
      }
      else
      {
         if(!TryMoveX(this, x + vx)) vx = 0;
         if(!TryMoveY(this, y + vy)) vy = 0;
         if(!TryMoveZ(this, z + vz)) vz = 0;
      }

      BlockMap::Root.insert(this);
   }

   //
   // PhysicsThinker::Collide
   //
   bool PhysicsThinker::Collide(PhysicsThinker *th, Coord &oldx, Coord &oldy, Coord &oldz)
   {
      bool collided = false;

      auto const &findRes = BlockMap::Root.find(th);

      // Collide with sectors.
      for(auto sec : findRes.sec)
      {
         // If inside sector's opening, no collision.
         if(th->z - th->sz >= sec->zl && th->z + th->sz <= sec->zh)
            continue;

         // If outside sector's outer box, no collision.
         if((th->x - th->sx >= sec->xh || th->x + th->sx <= sec->xl) ||
            (th->y - th->sy >= sec->yh || th->y + th->sy <= sec->yl))
            continue;

         // If not a rectangle, check against lines.
         if(!sec->rect)
         {
            // TODO
         }

         if(th->collideInto(sec))
         {
            // TODO: Snap position.

            collided = true;
         }
      }

      // Collide with thinkers.
      for(auto oth : findRes.th)
      {
         if(std::abs(th->x - oth->x) < th->sx + oth->sx &&
            std::abs(th->y - oth->y) < th->sy + oth->sy &&
            std::abs(th->z - oth->z) < th->sz + oth->sz)
         {
            // Always perform both collisions for side effects.
            if(th->collideInto(oth) & oth->collideFrom(th))
            {
               // TODO: Snap position.

               collided = true;
            }
         }
      }

      return collided;
   }

   //
   // PhysicsThinker::TryMoveX
   //
   bool PhysicsThinker::TryMoveX(PhysicsThinker *th, Coord x)
   {
      Coord oldx = th->x, oldy = th->y, oldz = th->z;

      th->x = x;

      if(Collide(th, oldx, oldy, oldz))
      {
         th->x = oldx;
         return false;
      }

      return true;
   }

   //
   // PhysicsThinker::TryMoveY
   //
   bool PhysicsThinker::TryMoveY(PhysicsThinker *th, Coord y)
   {
      Coord oldx = th->x, oldy = th->y, oldz = th->z;

      th->y = y;

      if(Collide(th, oldx, oldy, oldz))
      {
         th->y = oldy;
         return false;
      }

      return true;
   }

   //
   // PhysicsThinker::TryMoveZ
   //
   bool PhysicsThinker::TryMoveZ(PhysicsThinker *th, Coord z)
   {
      Coord oldx = th->x, oldy = th->y, oldz = th->z;

      th->z = z;

      if(Collide(th, oldx, oldy, oldz))
      {
         th->z = oldz;
         return false;
      }

      return true;
   }
}


//----------------------------------------------------------------------------|
// Natives                                                                    |
//

namespace DGE::Game
{
   //
   // unsigned DGE_CreatePhysicsThinker(unsigned ext)
   //
   DGE_Code_NativeDefn(DGE_CreatePhysicsThinker)
   {
      std::size_t ext = argv[0];

      task->dataStk.push((new(ext) PhysicsThinker)->id);
      return false;
   }

   //
   // void DGE_PhysicsThinkerBlock(unsigned id)
   //
   DGE_Code_NativeDefn(DGE_PhysicsThinkerBlock)
   {
      auto th = PhysicsThinker::Get(argv[0]);

      if(th) BlockMap::Root.insert(th);
      return false;
   }

   //
   // void DGE_PhysicsThinkerUnblock(unsigned id)
   //
   DGE_Code_NativeDefn(DGE_PhysicsThinkerUnblock)
   {
      auto th = PhysicsThinker::Get(argv[0]);

      if(th) BlockMap::Root.unlink(th);
      return false;
   }
}

// EOF

