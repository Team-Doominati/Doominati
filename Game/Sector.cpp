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
   void Sector_NativeAdd() {}

   //
   // Sector constructor
   //
   Sector::Sector(std::size_t pc_, Point2 *pv_) :
      blockLinks{this},
      pc{pc_}, pv{pv_},
      texc{0}, texf{0},
      frictair{0},
      friction{0},
      gx{0}, gy{0}, gz{0},
      xl{0}, yl{0}, xh{0}, yh{0},
      zl{0}, zh{0},
      rect{false}
   {
      // Hack to keep sectors around until such time as a proper ownership
      // relationship can be established.
      ++refCount;
   }

   //
   // Sector::calcBounds
   //
   void Sector::calcBounds()
   {
      // Add the first point to the end for algorithm convenience.
      pv[pc] = pv[0];

      // Calculate coordinate extremes.
      xl = xh = pv[0].x;
      yl = yh = pv[0].y;

      for(std::size_t i = 1; i != pc; ++i)
      {
         xl = std::min(xl, pv[i].x);
         xh = std::max(xh, pv[i].x);
         yl = std::min(yl, pv[i].y);
         yh = std::max(yh, pv[i].y);
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
   Sector::Ref Sector::Create(std::size_t pc, std::size_t ext)
   {
      // Allocate storage.
      // Memory layout: [Sector] [extension members] [points]
      // This requires Code::Word having equal or stricter alignment to Point2.
      Sector *sec = static_cast<Sector *>(
         operator new(sizeof(Sector) + (pc + 1) * sizeof(Point2), ext));

      auto pbuf = reinterpret_cast<Point2 *>(reinterpret_cast<Code::Word *>(sec + 1) + ext);

      return Ref{new(sec) Sector{pc, pbuf}};
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

