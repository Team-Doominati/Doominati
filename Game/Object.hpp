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

#include "../Game/Types.hpp"

#include "../Core/Alloc.hpp"
#include "../Core/IDAllocator.hpp"
#include "../Core/String.hpp"

#include "../Code/ExtMem.hpp"

#include <GDCC/Core/Counter.hpp>


//----------------------------------------------------------------------------|
// Macros                                                                     |
//

//
// DGE_Game_Object_GetMemberCases
//
#define DGE_Game_Object_GetMemberCases() \
   case ObjectMember::emc: return emc; \
   case ObjectMember::id:  return id

//
// DGE_Game_Object_SetMemberCases
//
#define DGE_Game_Object_SetMemberCases() \
   case ObjectMember::emc: (void)val; break; \
   case ObjectMember::id:  (void)val; break

//
// DGE_Game_ObjectImplement
//
#define DGE_Game_ObjectImplement(name) \
   DGE_Game_ObjectImplementCommon(name); \
   \
   Code::ExtensionMembers name::ExtMem{#name, &Super::ExtMem}

//
// DGE_Game_ObjectImplementCommon
//
#define DGE_Game_ObjectImplementCommon(name) \
   Code::Word name::getMember(ObjectMember mem) \
      {switch(mem) {DGE_Game_##name##_GetMemberCases(); \
         default: return 0;}} \
   void name::setMember(ObjectMember mem, Code::Word val) \
      {switch(mem) {DGE_Game_##name##_SetMemberCases(); \
         default: (void)val; break;}} \
   \
   static Object::ObjectTypeAdder name##TypeAdd{#name, \
      [](Object *o) {return dynamic_cast<name *>(o) ? o->id : 0;}}

//
// DGE_Game_ObjectPreamble
//
#define DGE_Game_ObjectPreamble(name, base) \
   GDCC_Core_CounterPreambleNoClone(DGE::Game::name, DGE::Game::base); \
   DGE_Game_ObjectPreambleCommon(name)

//
// DGE_Game_ObjectPreambleCommon
//
#define DGE_Game_ObjectPreambleCommon(name) \
public: \
   virtual Code::Word getMember(ObjectMember mem); \
   virtual void setMember(ObjectMember mem, Code::Word val); \
   \
   static Code::ExtensionMembers ExtMem;\
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
      //
      // ObjectTypeAdder
      //
      struct ObjectTypeAdder
      {
         ObjectTypeAdder(Core::HashedStr name, Code::Word (*func)(Object *));


         static void Finish();
      };


      void refAdd() {++refCount;}

      void refSub() {if(!--refCount) delete this;}

      Code::Word *const emv;
      Code::Word  const emc;
      Code::Word  const id;


      //
      // CreateT
      //
      template<typename T>
      static T *CreateT(Code::Word ext)
      {
         std::size_t emc = T::ExtMem.max() + ext;

         // Memory layout: [T] [extension members]
         auto emo = Core::AlignOffset<Code::Word>(sizeof(T));
         auto buf = static_cast<char *>(::operator new(emo + emc * sizeof(Code::Word)));
         auto emv = reinterpret_cast<Code::Word *>(buf + emo);

         return new(buf) T{emv, emc};
      }

   protected:
      Object(Code::Word *emv, std::size_t emc);
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

