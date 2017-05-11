//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Playsim objects
//
//-----------------------------------------------------------------------------

#ifndef DGE__Game__Object_H__
#define DGE__Game__Object_H__

#include "Core/IDAllocator.hpp"

#include "Code/Types.hpp"

#include <GDCC/Core/Counter.hpp>


//----------------------------------------------------------------------------|
// Macros                                                                     |
//

//
// DGE_Game_Object_GetMemberCases
//
#define DGE_Game_Object_GetMemberCases() \
   case ObjectMember::id: return id

//
// DGE_Game_Object_SetMemberCases
//
#define DGE_Game_Object_SetMemberCases() \
   case ObjectMember::id: (void)val; break

//
// DGE_Game_ObjectPreamble
//
#define DGE_Game_ObjectPreamble(name, base) \
   GDCC_Core_CounterPreambleNoClone(DGE::Game::name, DGE::Game::base); \
   DGE_Game_ObjectPreambleCommon(name); \
   \
public: \
   static std::size_t GetExtMemCF() \
      {return ExtMemCF = Super::GetExtMemCF() + ExtMemC;}

//
// DGE_Game_ObjectPreambleCommon
//
#define DGE_Game_ObjectPreambleCommon(name) \
public: \
   void *operator new(std::size_t size, std::size_t ext = 0) \
      {return ::operator new(size + (ExtMemCF + ext) * sizeof(Code::Word));} \
   void *operator new(std::size_t, void *ptr) {return ptr;} \
   \
   virtual Code::Word *extMember() \
      {return reinterpret_cast<Code::Word *>(this + 1);} \
   virtual Code::Word getMember(ObjectMember mem) \
      {switch(mem) {DGE_Game_##name##_GetMemberCases(); \
         default: return This::extMember()[mem - ObjectMember::MAX];}} \
   virtual void setMember(ObjectMember mem, Code::Word val) \
      {switch(mem) {DGE_Game_##name##_SetMemberCases(); \
         default: This::extMember()[mem - ObjectMember::MAX] = val;}} \
   \
   static inline std::size_t ExtMemC; \
   static inline std::size_t ExtMemCF; \
   \
   static This *Get(Code::Word id) \
      {return dynamic_cast<This *>(ObjectVec[id]);}


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::Game
{
   //
   // ObjectMember
   //
   enum class ObjectMember : Code::Word
   {
      #define DGE_Game_ObjectMemberList(name) name,
      #include "Game/ObjectMemberList.hpp"

      MAX
   };

   constexpr Code::Word operator - (ObjectMember l, ObjectMember r);

   //
   // Object
   //
   class Object : public GDCC::Core::Counter
   {
      GDCC_Core_CounterPreambleNoClone(DGE::Game::Object, GDCC::Core::Counter);
      DGE_Game_ObjectPreambleCommon(Object);

   public:
      void refAdd() {++refCount;}

      void refSub() {if(!--refCount) delete this;}

      Code::Word id;


      static std::size_t GetExtMemCF() {return ExtMemCF = ExtMemC;}

   protected:
      Object();
      virtual ~Object();


      static Core::IDAllocator<Object, Code::Word> &GetObjectVec();

      static Core::IDAllocator<Object, Code::Word> &ObjectVec;
   };
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Game
{
   //
   // operator ObjectMember - ObjectMember
   //
   constexpr Code::Word operator - (ObjectMember l, ObjectMember r)
   {
      return static_cast<Code::Word>(l) - static_cast<Code::Word>(r);
   }
}

#endif//DGE__Game__Object_H__

