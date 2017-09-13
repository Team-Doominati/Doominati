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
   DGE_Game_ThinkerImplement(PhysicsThinker);

   //
   // PhysicsThinker::applyFriction
   //
   void PhysicsThinker::applyFriction(Fract f)
   {
      if(!(f *= friction))
         return;

      if(mass)
      {
         vx -= Coord::MulFB(vx, f) / mass;
         vy -= Coord::MulFB(vy, f) / mass;
         vz -= Coord::MulFB(vz, f) / mass;
      }
      else
         vx = vy = vz = 0;
   }

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

      // Gravity and air friction.
      if(auto sec = BlockMap::Root.findSector(this))
      {
         vx += sec->gx;
         vy += sec->gy;
         vz += sec->gz;

         applyFriction(sec->frictair);
      }

      // Apply velocity.
      BlockMap::Root.unlink(this);

      Fract frict{0};
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
            if(stepx && !TryMoveX(this, x + stepx, frict))
            {
               if(bvx && avx >= bvz) vx = -vx, stepx = -stepx;
               else {stepx = vx = 0; if(avy + avz <= grabx) stepy = stepz = vy = vz = 0;}
            }

            if(stepy && !TryMoveY(this, y + stepy, frict))
            {
               if(bvy && avy >= bvy) vy = -vy, stepy = -stepy;
               else {stepy = vy = 0; if(avx + avz <= graby) stepx = stepz = vx = vz = 0;}
            }

            if(stepz && !TryMoveZ(this, z + stepz, frict))
            {
               if(bvz && avz >= bvz) vz = -vz, stepz = -stepz;
               else {stepz = vz = 0; if(avx + avy <= grabz) stepx = stepy = vx = vy = 0;}
            }
         }
      }
      else
      {
         if(vx && !TryMoveX(this, x + vx, frict))
         {
            if(bvx && avx >= bvx) vx = -vx;
            else {vx = 0; if(avy + avz <= grabx) vy = vz = 0;}
         }

         if(vy && !TryMoveY(this, y + vy, frict))
         {
            if(bvy && avy >= bvy) vy = -vy;
            else {vy = 0; if(avx + avz <= graby) vx = vz = 0;}
         }

         if(vz && !TryMoveZ(this, z + vz, frict))
         {
            if(bvz && avz >= bvz) vz = -vz;
            else {vz = 0; if(avx + avy <= grabz) vx = vy = 0;}
         }
      }

      applyFriction(frict);

      BlockMap::Root.insert(this);
   }

   //
   // PhysicsThinker::Collide
   //
   bool PhysicsThinker::Collide(PhysicsThinker *th, Coord &oldx, Coord &oldy, Coord &oldz, Fract &friction)
   {
      Coord snapx    = th->x;
      Coord snapy    = th->y;
      Coord snapz    = th->z;
      bool  collided = false;

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
            // Snap position.
            if(sec->rect)
            {
               if(oldx < th->x) snapx = std::min(snapx, sec->xl - th->sx);
               if(oldx > th->x) snapx = std::max(snapx, sec->xh + th->sx);
               if(oldy < th->y) snapy = std::min(snapy, sec->yl - th->sy);
               if(oldy > th->y) snapy = std::max(snapy, sec->yh + th->sy);
               if(oldz < th->z) snapz = std::min(snapz, sec->zl + th->sz);
               if(oldz > th->z) snapz = std::max(snapz, sec->zh - th->sz);
            }
            else
            {
               // TODO
            }

            if(friction < sec->friction)
               friction = sec->friction;

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
               // Snap position.
               if(oldx < th->x) snapx = std::min(snapx, oth->x - oth->sx - th->sx);
               if(oldx > th->x) snapx = std::max(snapx, oth->x + oth->sx + th->sx);
               if(oldy < th->y) snapy = std::min(snapy, oth->y - oth->sy - th->sy);
               if(oldy > th->y) snapy = std::max(snapy, oth->y + oth->sy + th->sy);
               if(oldz < th->z) snapz = std::min(snapz, oth->z - oth->sz - th->sz);
               if(oldz > th->z) snapz = std::max(snapz, oth->z + oth->sz + th->sz);

               if(friction < oth->friction)
                  friction = oth->friction;

               collided = true;
            }
         }
      }

      oldx = snapx;
      oldy = snapy;
      oldz = snapz;

      return collided;
   }

   //
   // PhysicsThinker::TryMoveX
   //
   bool PhysicsThinker::TryMoveX(PhysicsThinker *th, Coord x, Fract &friction)
   {
      Coord oldx = th->x, oldy = th->y, oldz = th->z;

      th->x = x;

      if(Collide(th, oldx, oldy, oldz, friction))
      {
         th->x = oldx;
         return false;
      }

      return true;
   }

   //
   // PhysicsThinker::TryMoveY
   //
   bool PhysicsThinker::TryMoveY(PhysicsThinker *th, Coord y, Fract &friction)
   {
      Coord oldx = th->x, oldy = th->y, oldz = th->z;

      th->y = y;

      if(Collide(th, oldx, oldy, oldz, friction))
      {
         th->y = oldy;
         return false;
      }

      return true;
   }

   //
   // PhysicsThinker::TryMoveZ
   //
   bool PhysicsThinker::TryMoveZ(PhysicsThinker *th, Coord z, Fract &friction)
   {
      Coord oldx = th->x, oldy = th->y, oldz = th->z;

      th->z = z;

      if(Collide(th, oldx, oldy, oldz, friction))
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
   // void DGE_PhysicsThinker_Block(unsigned id)
   //
   DGE_Code_NativeDefn(DGE_PhysicsThinker_Block)
   {
      auto th = PhysicsThinker::Get(argv[0]);

      if(th) BlockMap::Root.insert(th);
      return false;
   }

   //
   // unsigned DGE_PhysicsThinker_Create(unsigned ext)
   //
   DGE_Code_NativeDefn(DGE_PhysicsThinker_Create)
   {
      std::size_t ext = argv[0];

      task->dataStk.push((new(ext) PhysicsThinker)->id);
      return false;
   }

   //
   // void DGE_PhysicsThinker_Unblock(unsigned id)
   //
   DGE_Code_NativeDefn(DGE_PhysicsThinker_Unblock)
   {
      auto th = PhysicsThinker::Get(argv[0]);

      if(th) BlockMap::Root.unlink(th);
      return false;
   }
}

// EOF

