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
#include "Game/MemInfo.hpp"

#include "Code/MemPtr.hpp"
#include "Code/Native.hpp"
#include "Code/Program.hpp"
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
         auto getf = [f, this](Fixed v) -> Fixed
         {
            auto vf = Fixed::MulFB(v, f);
            return vf ? vf : v < 0
               ? -std::numeric_limits<Fixed>::epsilon()
               :  std::numeric_limits<Fixed>::epsilon();
         };

         if(vx) vx -= getf(vx);
         if(vy) vy -= getf(vy);
         if(vz) vz -= getf(vz);
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
         vx += sec->gx * gx;
         vy += sec->gy * gy;
         vz += sec->gz * gz;

         applyFriction(sec->frictair);
      }

      // Apply velocity.
      BlockMap::Root.unlink(this);

      Fract frict{0};
      auto avx = std::abs(vx);
      auto avy = std::abs(vy);
      auto avz = std::abs(vz);
      // Never move more than one width at a time.
      if(avx > sx || avy > sy || avz > sz)
      {
         Fixed stepx, stepy, stepz;
         Fixed::I steps;

         steps = 1 + static_cast<Fixed::I>(std::max({
            avx / std::max(sx, std::numeric_limits<Fixed>::epsilon() * 2),
            avy / std::max(sy, std::numeric_limits<Fixed>::epsilon() * 2),
            avz / std::max(sz, std::numeric_limits<Fixed>::epsilon() * 2)}));
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
   bool PhysicsThinker::Collide(PhysicsThinker *th, Fixed &oldx, Fixed &oldy, Fixed &oldz, Fract &friction)
   {
      Fixed snapx    = th->x;
      Fixed snapy    = th->y;
      Fixed snapz    = th->z;
      bool  collided = false;

      auto const &findRes = BlockMap::Root.find(th);

      // Collide with sectors.
      for(auto sec : findRes.sec)
      {
         // If inside sector's opening, no collision.
         if(th->z - th->sz >= sec->zl && th->z + th->sz <= sec->zu)
            continue;

         // If outside sector's outer box, no collision.
         if((th->x - th->sx >= sec->xu || th->x + th->sx <= sec->xl) ||
            (th->y - th->sy >= sec->yu || th->y + th->sy <= sec->yl))
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
               if(oldx > th->x) snapx = std::max(snapx, sec->xu + th->sx);
               if(oldy < th->y) snapy = std::min(snapy, sec->yl - th->sy);
               if(oldy > th->y) snapy = std::max(snapy, sec->yu + th->sy);
               if(oldz < th->z) snapz = std::min(snapz, sec->zu - th->sz);
               if(oldz > th->z) snapz = std::max(snapz, sec->zl + th->sz);
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
   bool PhysicsThinker::TryMoveX(PhysicsThinker *th, Fixed x, Fract &friction)
   {
      Fixed oldx = th->x, oldy = th->y, oldz = th->z;

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
   bool PhysicsThinker::TryMoveY(PhysicsThinker *th, Fixed y, Fract &friction)
   {
      Fixed oldx = th->x, oldy = th->y, oldz = th->z;

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
   bool PhysicsThinker::TryMoveZ(PhysicsThinker *th, Fixed z, Fract &friction)
   {
      Fixed oldx = th->x, oldy = th->y, oldz = th->z;

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
   // void DGE_PhysicsThinker_ApplyFriction(unsigned id, long fract f)
   //
   DGE_Code_NativeDefn(DGE_PhysicsThinker_ApplyFriction)
   {
      PhysicsThinker::Get(argv[0])->applyFriction(Fract::Raw(argv[1]));
      return false;
   }

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
   // unsigned DGE_PhysicsThinker_Collide(unsigned id, accum *oldx,
   //    accum *oldy, accum *oldz, long fract *fric)
   //
   DGE_Code_NativeDefn(DGE_PhysicsThinker_Collide)
   {
      auto th = PhysicsThinker::Get(argv[0]);

      Code::MemPtr<Fixed> oldx{task->prog->memory, argv[1]};
      Code::MemPtr<Fixed> oldy{task->prog->memory, argv[2]};
      Code::MemPtr<Fixed> oldz{task->prog->memory, argv[3]};
      Code::MemPtr<Fract> fric{task->prog->memory, argc > 4 ? argv[4] : 0};

      Fixed tmpx = oldx ? *oldx : th->x;
      Fixed tmpy = oldy ? *oldy : th->y;
      Fixed tmpz = oldz ? *oldz : th->z;
      Fract tmpf = fric ? *fric : Fract{};

      task->dataStk.push(PhysicsThinker::Collide(th, tmpx, tmpy, tmpz, tmpf));

      if(oldx) *oldx = tmpx;
      if(oldy) *oldy = tmpy;
      if(oldz) *oldz = tmpz;
      if(fric) *fric = tmpf;

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

