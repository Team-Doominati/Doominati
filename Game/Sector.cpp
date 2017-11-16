//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Map geometry sectors.
//
//-----------------------------------------------------------------------------

#include "Game/Sector.hpp"

#include "Game/BlockMap.hpp"
#include "Game/MemInfo.hpp"

#include "Code/Native.hpp"
#include "Code/Program.hpp"
#include "Code/Task.hpp"

#include <algorithm>
#include <stdexcept>


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Game
{
   DGE_Code_NativeLoaderDefn(Sector);
   DGE_Game_ObjectImplement(Sector);

   //
   // Sector constructor
   //
   Sector::Sector(Code::Word *emv_, std::size_t emc_, Point2X *pv_, std::size_t pc_) :
      Object{emv_, emc_},
      blockLinks{this},
      pc{pc_}, pv{pv_},
      texc{0}, texf{0},
      frictair{0},
      friction{0},
      gx{0}, gy{0}, gz{0},
      xl{0}, yl{0}, xu{0}, yu{0},
      zl{0}, zu{0},
      rect{false}
   {
      std::uninitialized_value_construct_n(pv, pc);
   }

   //
   // Sector::calcBounds
   //
   void Sector::calcBounds()
   {
      // Add the first point to the end for algorithm convenience.
      pv[pc] = pv[0];

      // Calculate coordinate extremes.
      xl = xu = pv[0].x;
      yl = yu = pv[0].y;

      for(std::size_t i = 1; i != pc; ++i)
      {
         xl = std::min(xl, pv[i].x);
         xu = std::max(xu, pv[i].x);
         yl = std::min(yl, pv[i].y);
         yu = std::max(yu, pv[i].y);
      }

      // Check for rectangular sector.
      rect = pc == 4 &&
         (pv[0].x == pv[1].x || pv[0].y == pv[1].y) &&
         (pv[1].x == pv[2].x || pv[1].y == pv[2].y) &&
         (pv[2].x == pv[3].x || pv[2].y == pv[3].y) &&
         (pv[3].x == pv[0].x || pv[3].y == pv[0].y);
   }

   //
   // Sector::Create
   //
   Sector *Sector::Create(std::size_t pc, std::size_t ext)
   {
      std::size_t emc = ExtMem.max() + ext;

      // Memory layout: [Sector] [extension members] [points]

      std::size_t emo = Core::AlignOffset<Code::Word>(sizeof(Sector));
      std::size_t po  = Core::AlignOffset<Point2X>(emo + emc * sizeof(Code::Word));

      auto buf = static_cast<char *>(::operator new(po + (pc + 1) * sizeof(Point2X)));

      auto emv = reinterpret_cast<Code::Word *>(buf + emo);
      auto pv  = reinterpret_cast<Point2X    *>(buf + po);

      return new(buf) Sector{emv, emc, pv, pc};
   }
}


//----------------------------------------------------------------------------|
// Natives                                                                    |
//

namespace DGE::Game
{
   //
   // void DGE_Sector_Block(unsigned id)
   //
   DGE_Code_NativeDefn(DGE_Sector_Block)
   {
      if(auto sec = Sector::Get(argv[0])) BlockMap::Root.insert(sec);
      return false;
   }

   //
   // unsigned DGE_Sector_Create(unsigned pc, unsigned ext)
   //
   DGE_Code_NativeDefn(DGE_Sector_Create)
   {
      std::size_t pc  = argv[0];
      std::size_t ext = argv[1];

      task->dataStk.push(Sector::Create(pc, ext)->id);
      return false;
   }

   //
   // void DGE_Sector_CalcBounds(unsigned id)
   //
   DGE_Code_NativeDefn(DGE_Sector_CalcBounds)
   {
      if(auto sec = Sector::Get(argv[0])) sec->calcBounds();
      return false;
   }

   //
   // DGE_Point2 DGE_Sector_PointGet(unsigned id, unsigned p)
   //
   DGE_Code_NativeDefn(DGE_Sector_PointGet)
   {
      auto const &p = Sector::Get(argv[0])->pv[argv[1]];
      task->dataStk.push(p.x.raw());
      task->dataStk.push(p.y.raw());
      return false;
   }

   //
   // void DGE_Sector_PointSet(unsigned id, unsigned p, DGE_Point2 val)
   //
   DGE_Code_NativeDefn(DGE_Sector_PointSet)
   {
      auto &p = Sector::Get(argv[0])->pv[argv[1]];
      p.x.raw(argv[2]);
      p.y.raw(argv[3]);
      return false;
   }

   //
   // void DGE_Sector_Unblock(unsigned id)
   //
   DGE_Code_NativeDefn(DGE_Sector_Unblock)
   {
      if(auto sec = Sector::Get(argv[0])) BlockMap::Root.unlink(sec);
      return false;
   }
}

// EOF

