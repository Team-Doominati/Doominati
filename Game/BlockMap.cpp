//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// BlockMaps.
//
//-----------------------------------------------------------------------------

#include "Game/BlockMap.hpp"

#include "Code/Native.hpp"
#include "Code/Task.hpp"

#include <vector>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::Game
{
   //
   // FindResNative
   //
   class FindResNative
   {
   public:
      FindResNative() : sec{nullptr}, th{nullptr} {}

      void free() {id.clear(); sec = nullptr; th = nullptr;}

      Code::Word getSec(Code::Word idx) const {return idx < secC ? sec[idx] : 0;}
      Code::Word getTh(Code::Word idx) const {return idx < thC ? th[idx] : 0;}

      //
      // init
      //
      void init(BlockMap::FindRes const &res)
      {
         secC = res.sec.size();
         thC  = res.th.size();
         id.resize(secC + thC);
         sec = &id[0];
         th  = &id[secC];

         auto itr = id.begin();
         for(auto o : res.sec) *itr++ = o->id;
         for(auto o : res.th)  *itr++ = o->id;
      }

      std::vector<Code::Word> id;
      Code::Word *sec;
      std::size_t secC;
      Code::Word *th;
      std::size_t thC;


      //
      // Get
      //
      static FindResNative &Get(Code::Word idx)
      {
         return idx < Buf.size() ? Buf[idx] : GetFree();
      }

      //
      // GetFree
      //
      static FindResNative &GetFree()
      {
         for(auto &buf : Buf)
            if(!buf.sec) return buf;

         return Buf.emplace_back();
      }

      static std::vector<FindResNative> Buf;
   };
}


//----------------------------------------------------------------------------|
// Static Objects                                                             |
//

namespace DGE::Game
{
   static BlockMap::FindRes FindResBuf;
}


//----------------------------------------------------------------------------|
// Extern Objects                                                             |
//

namespace DGE::Game
{
   BlockMap BlockMap::Root{0, 0, 0x800000};

   std::vector<FindResNative> FindResNative::Buf;
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Game
{
   //
   // BlockMap::countNode
   //
   std::size_t BlockMap::countNode(Coord xl, Coord yl, Coord xh, Coord yh)
   {
      if(!subs)
         return 1;

      std::size_t count = 1;

      if(xl < cx && yh > cy) count += subs[0].countNode(xl, yl, xh, yh);
      if(xl < cx && yl < cy) count += subs[2].countNode(xl, yl, xh, yh);
      if(xh > cx && yh > cy) count += subs[1].countNode(xl, yl, xh, yh);
      if(xh > cx && yl < cy) count += subs[3].countNode(xl, yl, xh, yh);

      return count;
   }

   //
   // BlockMap::find
   //
   BlockMap::FindRes const &BlockMap::find(Coord xl, Coord yl, Coord xh, Coord yh)
   {
      FindResBuf.th.clear();
      FindResBuf.sec.clear();

      forNode(xl, yl, xh, yh, [](BlockMap *node)
      {
         for(auto &sec : node->listSec)
            FindResBuf.sec.insert(&sec);

         for(auto &th : node->listTh)
            FindResBuf.th.insert(&th);
      });

      return FindResBuf;
   }

   //
   // BlockMap::insert
   //
   void BlockMap::insert(PhysicsThinker *th)
   {
      th->blockLinks.resize(countNode(th));
      auto iter = th->blockLinks.begin();
      insertNode(th, iter);
   }

   //
   // BlockMap::insert
   //
   void BlockMap::insert(Sector *sec)
   {
      sec->blockLinks.resize(countNode(sec));
      auto iter = sec->blockLinks.begin();
      insertNode(sec, iter);
   }

   //
   // BlockMap::unlink
   //
   void BlockMap::unlink(PhysicsThinker *th)
   {
      th->blockLinks.unlink();
   }

   //
   // BlockMap::unlink
   //
   void BlockMap::unlink(Sector *sec)
   {
      sec->blockLinks.unlink();
   }
}


//----------------------------------------------------------------------------|
// Natives                                                                    |
//

namespace DGE::Game
{
   //
   // unsigned DGE_BlockMap_Find(accum xl, accum yl, accum xh, accum yh)
   //
   DGE_Code_NativeDefn(DGE_BlockMap_Find)
   {
      Coord xl{static_cast<Code::SWord>(argv[0]), Core::FixedRaw};
      Coord yl{static_cast<Code::SWord>(argv[1]), Core::FixedRaw};
      Coord xh{static_cast<Code::SWord>(argv[2]), Core::FixedRaw};
      Coord yh{static_cast<Code::SWord>(argv[3]), Core::FixedRaw};

      auto &res = FindResNative::GetFree();
      res.init(BlockMap::Root.find(xl, yl, xh, yh));
      task->dataStk.push(&res - FindResNative::Buf.data());

      return false;
   }

   //
   // unsigned DGE_BlockMap_FindCountSector(unsigned find)
   //
   DGE_Code_NativeDefn(DGE_BlockMap_FindCountSector)
   {
      task->dataStk.push(FindResNative::Get(argv[0]).secC);
      return false;
   }

   //
   // unsigned DGE_BlockMap_FindCountThinker(unsigned find)
   //
   DGE_Code_NativeDefn(DGE_BlockMap_FindCountThinker)
   {
      task->dataStk.push(FindResNative::Get(argv[0]).thC);
      return false;
   }

   //
   // void DGE_BlockMap_FindFree(unsigned find)
   //
   DGE_Code_NativeDefn(DGE_BlockMap_FindFree)
   {
      FindResNative::Get(argv[0]).free();
      return false;
   }

   //
   // unsigned DGE_BlockMap_FindGetSector(unsigned find, unsigned idx)
   //
   DGE_Code_NativeDefn(DGE_BlockMap_FindGetSector)
   {
      task->dataStk.push(FindResNative::Get(argv[0]).getSec(argv[1]));
      return false;
   }

   //
   // unsigned DGE_BlockMap_FindGetThinker(unsigned find, unsigned idx)
   //
   DGE_Code_NativeDefn(DGE_BlockMap_FindGetThinker)
   {
      task->dataStk.push(FindResNative::Get(argv[0]).getTh(argv[1]));
      return false;
   }
}

// EOF

