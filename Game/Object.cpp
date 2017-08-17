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
      static Core::IDAllocator<Object, Code::Word> vec{1};
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
   // T DGE_Object_MemberGet*(unsigned id, unsigned mem)
   //
   #define DGE_Game_Object_MemberGetDefn(suffix) \
      DGE_Code_NativeDefn(DGE_Object_MemberGet##suffix) \
      { \
         Object *obj = Object::Get(argv[0]); \
         auto    mem = static_cast<ObjectMember>(argv[1]); \
         \
         task->dataStk.push(obj ? obj->getMember(mem) : 0); \
         return false; \
      }

   DGE_Game_Object_MemberGetDefn(I)
   DGE_Game_Object_MemberGetDefn(LA)
   DGE_Game_Object_MemberGetDefn(U)
   DGE_Game_Object_MemberGetDefn(X)

   //
   // void DGE_Object_MemberSet*(unsigned id, unsigned mem, T val)
   //
   #define DGE_Game_Object_MemberSetDefn(suffix) \
      DGE_Code_NativeDefn(DGE_Object_MemberSet##suffix) \
      { \
         Object *obj = Object::Get(argv[0]); \
         auto    mem = static_cast<ObjectMember>(argv[1]); \
         \
         if(obj) obj->setMember(mem, argv[2]); \
         return false; \
      }

   DGE_Game_Object_MemberSetDefn(I)
   DGE_Game_Object_MemberSetDefn(LA)
   DGE_Game_Object_MemberSetDefn(U)
   DGE_Game_Object_MemberSetDefn(X)

   //
   // void DGE_Object_RefAdd(unsigned id)
   //
   DGE_Code_NativeDefn(DGE_Object_RefAdd)
   {
      if(auto obj = Object::Get(argv[0])) obj->refAdd();
      return false;
   }

   //
   // void DGE_Object_RefSub(unsigned id)
   //
   DGE_Code_NativeDefn(DGE_Object_RefSub)
   {
      if(auto obj = Object::Get(argv[0])) obj->refSub();
      return false;
   }
}

// EOF

