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


//----------------------------------------------------------------------------|
// Extern Objects                                                             |
//

namespace DGE::Game
{
   BlockMap BlockMap::Root{0, 0, 0x800000};
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Game
{
   //
   // BlockMap::getNode
   //
   BlockMap *BlockMap::getNode(Coord x, Coord y)
   {
      return subs ? subs[(x >= cx) | ((y >= cy) << 1)].getNode(x, y) : this;
   }

   //
   // BlockMap::insert
   //
   void BlockMap::insert(PhysicsThinker *th)
   {
      std::size_t count = 0;
      forNodeAll(th, [&count](BlockMap *){++count;});

      th->blockLinks.resize(count);

      auto itr = th->blockLinks.begin();
      forNodeAll(th, [&itr](BlockMap *node){itr++->insert(&node->listTh);});
   }

   //
   // BlockMap::unlink
   //
   void BlockMap::unlink(PhysicsThinker *th)
   {
      th->blockLinks.unlink();
   }
}

// EOF

