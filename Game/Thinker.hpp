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
   case ThinkerMember::id:   return id; \
   case ThinkerMember::next: return next->id; \
   case ThinkerMember::prev: return prev->id

//
// DGE_Game_Thinker_SetMemberCases
//
#define DGE_Game_Thinker_SetMemberCases() \
   case ThinkerMember::id:   (void)val; break; \
   case ThinkerMember::next: (void)val; break; \
   case ThinkerMember::prev: (void)val; break

//
// DGE_Game_ThinkerPreamble
//
#define DGE_Game_ThinkerPreamble(name, base) \
   DGE_Game_ObjectPreamble(name, base) \
   DGE_Game_ThinkerPreambleCommon(name); \
   \
public: \
   using Range = RangeT<name>; \
   \
   static std::size_t GetExtMemCF() \
      {return ExtMemCF = Super::GetExtMemCF() + ExtMemC;}

//
// DGE_Game_ThinkerPreambleCommon
//
#define DGE_Game_ThinkerPreambleCommon(name) \
public: \
   void *operator new(std::size_t size, std::size_t ext = 0) \
      {return ::operator new(size + (ExtMemCF + ext) * sizeof(Code::Word));} \
   \
   virtual Code::Word *extMember() \
      {return reinterpret_cast<Code::Word *>(this + 1);} \
   virtual Code::Word getMember(ThinkerMember mem) \
      {switch(mem) {DGE_Game_##name##_GetMemberCases(); \
         default: return This::extMember()[mem - ThinkerMember::MAX];}} \
   virtual void setMember(ThinkerMember mem, Code::Word val) \
      {switch(mem) {DGE_Game_##name##_SetMemberCases(); \
         default: This::extMember()[mem - ThinkerMember::MAX] = val;}} \
   \
   static inline std::size_t ExtMemC; \
   static inline std::size_t ExtMemCF


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
      #define DGE_Game_ThinkerMemberList(name) name,
      #include "Game/ThinkerMemberList.hpp"

      MAX
   };

   constexpr Code::Word operator - (ThinkerMember l, ThinkerMember r);

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

      static std::size_t GetExtMemCF() {return ExtMemCF = ExtMemC;}

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

   //
   // operator ThinkerMember - ThinkerMember
   //
   constexpr Code::Word operator - (ThinkerMember l, ThinkerMember r)
   {
      return static_cast<Code::Word>(l) - static_cast<Code::Word>(r);
   }
}

#endif//DGE__Game__Thinker_H__

