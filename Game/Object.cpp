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

#include "Game/Object.hpp"

#include "Code/Glyph.hpp"
#include "Code/Native.hpp"
#include "Code/Task.hpp"


//----------------------------------------------------------------------------|
// Extern Objects                                                             |
//

namespace DGE::Game
{
   Core::IDAllocator<Object, Code::Word> &Object::ObjectVec = GetObjectVec();
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Game
{
   //
   // Object constructor
   //
   Object::Object() :
      id{GetObjectVec().alloc(this)}
   {
   }

   //
   // Object destructor
   //
   Object::~Object()
   {
      ObjectVec.free(id);
   }

   //
   // Object::GetObjectVec
   //
   Core::IDAllocator<Object, Code::Word> &Object::GetObjectVec()
   {
      static Core::IDAllocator<Object, Code::Word> vec;
      return vec;
   }
}


//----------------------------------------------------------------------------|
// Glyph Types                                                                |
//

namespace DGE::Game
{
   //
   // {ObjectMember}
   //
   DGE_Code_GlyphTypeDefn(ObjectMember)
   {
      static Core::HashMapFixed<Core::HashedStr, ObjectMember> members
      {
         #define DGE_Game_ObjectMemberList(name) {#name, ObjectMember::name},
         #include "Game/ObjectMemberList.hpp"
      };

      if(auto *mem = members.find(glyph))
         return static_cast<Code::Word>(*mem);

      throw Code::GlyphError{"ObjectMember", glyph};
   }
}


//----------------------------------------------------------------------------|
// Natives                                                                    |
//

namespace DGE::Game
{
   //
   // T DGE_ObjectMemberGet*(unsigned id, unsigned mem)
   //
   #define DGE_Game_ObjectMemberGetDefn(suffix) \
      DGE_Code_NativeDefn(DGE_ObjectMemberGet##suffix) \
      { \
         Object *obj = Object::Get(argv[0]); \
         auto    mem = static_cast<ObjectMember>(argv[1]); \
         \
         task->dataStk.push(obj ? obj->getMember(mem) : 0); \
         return false; \
      }

   DGE_Game_ObjectMemberGetDefn(LA)
   DGE_Game_ObjectMemberGetDefn(U)
   DGE_Game_ObjectMemberGetDefn(X)

   //
   // void DGE_ObjectMemberSet*(unsigned id, unsigned mem, T val)
   //
   #define DGE_Game_ObjectMemberSetDefn(suffix) \
      DGE_Code_NativeDefn(DGE_ObjectMemberSet##suffix) \
      { \
         Object *obj = Object::Get(argv[0]); \
         auto    mem = static_cast<ObjectMember>(argv[1]); \
         \
         if(obj) obj->setMember(mem, argv[2]); \
         return false; \
      }

   DGE_Game_ObjectMemberSetDefn(LA)
   DGE_Game_ObjectMemberSetDefn(U)
   DGE_Game_ObjectMemberSetDefn(X)

   //
   // void DGE_ObjectRefAdd(unsigned id)
   //
   DGE_Code_NativeDefn(DGE_ObjectRefAdd)
   {
      if(auto obj = Object::Get(argv[0])) obj->refAdd();
      return false;
   }

   //
   // void DGE_ObjectRefSub(unsigned id)
   //
   DGE_Code_NativeDefn(DGE_ObjectRefSub)
   {
      if(auto obj = Object::Get(argv[0])) obj->refSub();
      return false;
   }
}

// EOF

