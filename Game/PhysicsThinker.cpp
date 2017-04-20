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
      if(avx >= wx || avy >= wy || avz >= wz)
      {
         Coord stepx, stepy, stepz;
         Coord::I steps;

         // Never move more than one width at a time.
         steps = static_cast<Coord::I>(std::ceil(std::max({avx / wx, avy / wy, avz / wz}))) + 1;
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

      BlockMap::Root.forNode(th, [&](BlockMap *node)
      {
         for(auto &oth : node->listTh)
         {
            if(std::abs(th->x - oth.x) < th->wx + oth.wx &&
               std::abs(th->y - oth.y) < th->wy + oth.wy &&
               std::abs(th->z - oth.z) < th->wz + oth.wz)
            {
               // TODO: Snap position.

               collided = true;
            }
         }
      });

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

