//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Playsim state advancement.
//
//-----------------------------------------------------------------------------

#ifndef DGE__Game__Thinker_H__
#define DGE__Game__Thinker_H__

#include "Code/Types.hpp"

#include "Core/IDAllocator.hpp"

#include <GDCC/Core/Counter.hpp>

//
// DGE_Game_ThinkerMemberCases
//
#define DGE_Game_ThinkerMemberCases() \
   case ThinkerMember::id:   return id; \
   case ThinkerMember::next: return next->id; \
   case ThinkerMember::prev: return prev->id


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::Game
{
   //
   // ThinkerMember
   //
   enum class ThinkerMember : Code::Word
   {
      id,
      next,
      prev,
   };

   //
   // Thinker
   //
   class Thinker : public GDCC::Core::Counter
   {
      GDCC_Core_CounterPreambleNoClone(DGE::Game::Thinker, GDCC::Core::Counter);

   public:
      template<typename T> class Iterator;
      template<typename T> class Range;


      virtual Code::Word getMember(ThinkerMember mem);

      Ptr      next;
      Thinker *prev;

      Code::Word id;


      template<typename T> static T *Begin();

      static Thinker *Get(Code::Word id) {return ThinkerVec[id];}

      template<typename T>
      static T *Get(Code::Word id) {return dynamic_cast<T *>(ThinkerVec[id]);}

      template<typename T> static T *Next(T *th);

      static void ThinkAll();

      static Thinker ThinkerCap;

   protected:
      Thinker();
      virtual ~Thinker();

      virtual void think() {}

   private:
      explicit Thinker(int); // ThinkerCap constructor.


      static Core::IDAllocator<Thinker, Code::Word> ThinkerVec;
   };

   //
   // Thinker::Iterator
   //
   template<typename T>
   class Thinker::Iterator
   {
   public:
      T *operator -> () {return th;}
      T &operator * () {return *th;}

      Iterator<T> operator ++ (int) {Iterator<T> i(*this); ++*this; return i;}

      Iterator<T> &operator ++ () {th = Next(th); return *this;}

      bool operator == (Iterator<T> const &i) {return th == i.th;}
      bool operator != (Iterator<T> const &i) {return th != i.th;}


      friend class Range<T>;

   private:
      explicit Iterator(T *th_) : th{th_} {}

      T *th;
   };

   //
   // Thinker::Range
   //
   template<typename T>
   class Thinker::Range
   {
   public:
      Iterator<T> begin() {return Iterator<T>(Begin<T>());}
      Iterator<T> end()   {return Iterator<T>(nullptr);}
   };
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Game
{
   //
   // Thinker::Begin
   //
   template<typename T>
   T *Thinker::Begin()
   {
      for(Thinker *it = ThinkerCap.next; it != &ThinkerCap; it = it->next)
      {
         if(T *th = dynamic_cast<T *>(it))
            return th;
      }

      return 0;
   }

   //
   // Thinker::Next
   //
   template<typename T>
   T *Thinker::Next(T *th)
   {
      for(Thinker *it = th->next; it != &ThinkerCap; it = it->next)
      {
         if((th = dynamic_cast<T *>(it)))
            return th;
      }

      return nullptr;
   }
}

#endif//DGE__Game__Thinker_H__

