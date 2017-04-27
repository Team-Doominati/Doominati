//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Objects that change over time.
//
//-----------------------------------------------------------------------------

#ifndef DGE__Game__Thinker_H__
#define DGE__Game__Thinker_H__

#include "Game/Object.hpp"


//----------------------------------------------------------------------------|
// Macros                                                                     |
//

//
// DGE_Game_Thinker_GetMemberCases
//
#define DGE_Game_Thinker_GetMemberCases() \
   DGE_Game_Object_GetMemberCases(); \
   case ObjectMember::next: return next->id; \
   case ObjectMember::prev: return prev->id

//
// DGE_Game_Thinker_SetMemberCases
//
#define DGE_Game_Thinker_SetMemberCases() \
   DGE_Game_Object_SetMemberCases(); \
   case ObjectMember::next: (void)val; break; \
   case ObjectMember::prev: (void)val; break

//
// DGE_Game_ThinkerPreamble
//
#define DGE_Game_ThinkerPreamble(name, base) \
   DGE_Game_ObjectPreamble(name, base); \
   DGE_Game_ThinkerPreambleCommon(name); \
   \
public: \
   using Range = RangeT<name>

//
// DGE_Game_ThinkerPreambleCommon
//
#define DGE_Game_ThinkerPreambleCommon(name)


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::Game
{
   //
   // Thinker
   //
   class Thinker : public Object
   {
      DGE_Game_ObjectPreamble(Thinker, Object);
      DGE_Game_ThinkerPreambleCommon(Thinker);

   public:
      template<typename T> class Iterator;
      template<typename T> class RangeT;

      using Range = RangeT<Thinker>;


      void unlink();

      Ptr      next;
      Thinker *prev;


      template<typename T> static T *Begin();

      template<typename T> static T *Next(T *th);

      static void ThinkAll();

      static Thinker ThinkerCap;

   protected:
      Thinker();
      virtual ~Thinker();

      virtual void think() {}

   private:
      explicit Thinker(int); // ThinkerCap constructor.
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


      friend class RangeT<T>;

   private:
      explicit Iterator(T *th_) : th{th_} {}

      T *th;
   };

   //
   // Thinker::RangeT
   //
   template<typename T>
   class Thinker::RangeT
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

