//-----------------------------------------------------------------------------
//
// Copyright (C) 2017-2019 Team Doominati
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

#include <unordered_set>


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
      //
      // FindRes
      //
      class FindRes
      {
      public:
         std::unordered_set<Sector *>         sec;
         std::unordered_set<PhysicsThinker *> th;
      };

      BlockMap(Fixed x, Fixed y, Fixed s) : subs{nullptr}, cx{x}, cy{y}, size{s} {}
      ~BlockMap();

      FindRes const &find(Fixed xl, Fixed yl, Fixed xu, Fixed yu);
      FindRes const &find(PhysicsThinker *th)
         {return find(th->x - th->sx, th->y - th->sy, th->x + th->sx, th->y + th->sy);}
      FindRes const &find(Sector *sec)
         {return find(sec->xl, sec->yl, sec->xu, sec->yu);}

      Sector::Ptr findSector(Fixed x, Fixed y);
      Sector::Ptr findSector(PhysicsThinker *th) {return findSector(th->x, th->y);}

      void insert(PhysicsThinker *th);
      void insert(Sector *sec);

      void split(Fixed minW, std::size_t maxObj);

      void unlink(PhysicsThinker *th);
      void unlink(Sector *sec);

      Core::ListLink<Sector>         listSec;
      Core::ListLink<PhysicsThinker> listTh;


      static BlockMap Root;

   private:
      // countNode
      std::size_t countNode(Fixed xl, Fixed yl, Fixed xu, Fixed yu);

      std::size_t countNode(PhysicsThinker *th)
         {return countNode(th->x - th->sx, th->y - th->sy, th->x + th->sx, th->y + th->sy);}

      std::size_t countNode(Sector *sec)
         {return countNode(sec->xl, sec->yl, sec->xu, sec->yu);}

      // countObj
      std::size_t countObj() {return listSec.size() + listTh.size();}

      // forNode
      template<typename Iter>
      void forNode(Fixed xl, Fixed yl, Fixed xu, Fixed yu, Iter &&iter);

      template<typename Iter>
      void forNode(PhysicsThinker *th, Iter &&iter)
         {return forNode(th->x - th->sx, th->y - th->sy, th->x + th->sx, th->y + th->sy, iter);}

      template<typename Iter>
      void forNode(Sector *sec, Iter &&iter)
         {return forNode(sec->xl, sec->yl, sec->xu, sec->yu, iter);}

      BlockMap *getNode(Fixed x, Fixed y);

      // insertNode
      template<typename T>
      void insertNode(Fixed xl, Fixed yl, Fixed xu, Fixed yu, Core::ListLink<T> *&iter);

      void insertNode(PhysicsThinker *th, Core::ListLink<PhysicsThinker> *&iter)
         {insertNode(th->x - th->sx, th->y - th->sy, th->x + th->sx, th->y + th->sy, iter);}

      void insertNode(Sector *sec, Core::ListLink<Sector> *&iter)
         {insertNode(sec->xl, sec->yl, sec->xu, sec->yu, iter);}

      template<typename T>
      Core::ListLink<T> &listHead();

      // relink
      void relink(PhysicsThinker *th) {unlink(th); insert(th);}
      void relink(Sector *sec) {unlink(sec); insert(sec);}

      BlockMap *subs;

      Fixed const cx, cy;
      Fixed const size;
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
   void BlockMap::forNode(Fixed xl, Fixed yl, Fixed xu, Fixed yu, Iter &&iter)
   {
      if(!subs)
         return (void)iter(this);

      // Check if box fully contains node.
      if(xl <= cx - size && xu >= cx + size && yl <= cy - size && yu >= cy + size)
         return (void)iter(this);

      if(xl <  cx && yl <  cy) subs[0].forNode(xl, yl, xu, yu, iter);
      if(xl <  cx && yu >= cy) subs[2].forNode(xl, yl, xu, yu, iter);
      if(xu >= cx && yl <  cy) subs[1].forNode(xl, yl, xu, yu, iter);
      if(xu >= cx && yu >= cy) subs[3].forNode(xl, yl, xu, yu, iter);
   }

   //
   // BlockMap::insertNode
   //
   template<typename T>
   void BlockMap::insertNode(Fixed xl, Fixed yl, Fixed xu, Fixed yu, Core::ListLink<T> *&iter)
   {
      iter++->insert(&listHead<T>());

      if(!subs)
         return;

      if(xl <  cx && yl <  cy) subs[0].insertNode(xl, yl, xu, yu, iter);
      if(xl <  cx && yu >= cy) subs[2].insertNode(xl, yl, xu, yu, iter);
      if(xu >= cx && yl <  cy) subs[1].insertNode(xl, yl, xu, yu, iter);
      if(xu >= cx && yu >= cy) subs[3].insertNode(xl, yl, xu, yu, iter);
   }

   //
   // BlockMap::listHead
   //
   template<> inline Core::ListLink<PhysicsThinker> &BlockMap::listHead<PhysicsThinker>() {return listTh;}
   template<> inline Core::ListLink<Sector> &BlockMap::listHead<Sector>() {return listSec;}
}

#endif//DGE__Game__BlockMap_H__

