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

#ifndef DGE__Game__BlockMap_H__
#define DGE__Game__BlockMap_H__

#include "Game/PhysicsThinker.hpp"
#include "Game/Sector.hpp"

#include <memory>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::Game
{
   //
   // BlockMap
   //
   // +---+
   // |0|1|
   // |-+-|
   // |2|3|
   // +---+
   //
   class BlockMap
   {
   public:
      BlockMap(Coord x, Coord y, Coord w) : cx{x}, cy{y}, width{w} {}

      BlockMap *getNode(Coord x, Coord y);
      BlockMap *getNode(Coord xl, Coord yl, Coord xh, Coord yh);

      // forNode
      template<typename Iter>
      void forNode(Coord xl, Coord yl, Coord xh, Coord yh, Iter &&iter);

      template<typename Iter>
      void forNode(PhysicsThinker *th, Iter &&iter)
         {return forNode(th->x - th->sx, th->y - th->sy, th->x + th->sx, th->y + th->sy, iter);}

      template<typename Iter>
      void forNode(Sector *sec, Iter &&iter)
         {return forNode(sec->xl, sec->yl, sec->xh, sec->yh, iter);}

      // forNodeAll
      template<typename Iter>
      void forNodeAll(Coord xl, Coord yl, Coord xh, Coord yh, Iter &&iter);

      template<typename Iter>
      void forNodeAll(PhysicsThinker *th, Iter &&iter)
         {return forNodeAll(th->x - th->sx, th->y - th->sy, th->x + th->sx, th->y + th->sy, iter);}

      template<typename Iter>
      void forNodeAll(Sector *sec, Iter &&iter)
         {return forNodeAll(sec->xl, sec->yl, sec->xh, sec->yh, iter);}

      void insert(PhysicsThinker *th);
      void insert(Sector *sec);

      void split();

      void unlink(PhysicsThinker *th);
      void unlink(Sector *sec);

      Core::ListLink<Sector>         listSec;
      Core::ListLink<PhysicsThinker> listTh;


      static BlockMap Root;

   private:
      std::unique_ptr<BlockMap[]> subs;

      Coord const cx, cy;
      Coord const width;
   };
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Game
{
   //
   // BlockMap::forNode
   //
   template<typename Iter>
   void BlockMap::forNode(Coord xl, Coord yl, Coord xh, Coord yh, Iter &&iter)
   {
      if(!subs)
         return (void)iter(this);

      // Check if box fully contains node.
      if(xl <= cx - width && xh >= cx + width && yl <= cy - width && yh >= cy + width)
         return (void)iter(this);

      if(xl < cx && yh > cy)
         subs[0].forNode(xl, yl, xh, yh, iter);

      if(xl < cx && yl < cy)
         subs[2].forNode(xl, yl, xh, yh, iter);

      if(xh > cx && yh > cy)
         subs[1].forNode(xl, yl, xh, yh, iter);

      if(xh > cx && yl < cy)
         subs[3].forNode(xl, yl, xh, yh, iter);
   }

   //
   // BlockMap::forNodeAll
   //
   template<typename Iter>
   void BlockMap::forNodeAll(Coord xl, Coord yl, Coord xh, Coord yh, Iter &&iter)
   {
      iter(this);

      if(!subs)
         return;

      if(xl < cx && yh > cy)
         subs[0].forNode(xl, yl, xh, yh, iter);

      if(xl < cx && yl < cy)
         subs[2].forNode(xl, yl, xh, yh, iter);

      if(xh > cx && yh > cy)
         subs[1].forNode(xl, yl, xh, yh, iter);

      if(xh > cx && yl < cy)
         subs[3].forNode(xl, yl, xh, yh, iter);
   }
}

#endif//DGE__Game__BlockMap_H__

