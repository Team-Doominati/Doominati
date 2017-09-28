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

#include "Code/ExtMem.hpp"
#include "Code/Glyph.hpp"
#include "Code/Native.hpp"
#include "Code/Task.hpp"

#include <vector>


//----------------------------------------------------------------------------|
// Static Objects                                                             |
//

namespace DGE::Game
{
   Core::HashMapFixed<Core::HashedStr, Code::Word (*)(Object *)> ObjectTypes;
}


//----------------------------------------------------------------------------|
// Extern Objects                                                             |
//

namespace DGE::Game
{
   Code::ExtensionMembers Object::ExtMem{"Object", nullptr};

   Core::IDAllocator<Object, Code::Word> &Object::ObjectVec = GetObjectVec();
}


//----------------------------------------------------------------------------|
// Static Functions                                                           |
//

namespace DGE::Game
{
   //
   // ObjectTypeVec
   //
   static auto &ObjectTypeVec()
   {
      static auto vec = new std::vector<std::pair<Core::HashedStr, Code::Word (*)(Object *)>>;
      return vec;
   }
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Game
{
   DGE_Game_ObjectImplementCommon(Object);

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

   //
   // Object::ObjectTypeAdder constructor
   //
   Object::ObjectTypeAdder::ObjectTypeAdder(Core::HashedStr name, Code::Word (*func)(Object *))
   {
      ObjectTypeVec()->emplace_back(name, func);
   }

   //
   // Object::ObjectTypeAdder::Finish
   //
   void Object::ObjectTypeAdder::Finish()
   {
      auto &vec = ObjectTypeVec();

      auto vecItr = vec->begin();
      ObjectTypes.reset(vec->size(),
         [&](auto elem){elem->key = vecItr->first; elem->val = vecItr->second; ++vecItr;});

      delete vec;
      vec = nullptr;
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

   //
   // {ObjectMemberExt}
   //
   DGE_Code_GlyphTypeDefn(ObjectMemberExt)
   {
      // Empty glyph means to give the extension member base.
      // Use of this is dangerous, but potentially useful for testing.
      if(glyph.empty()) return static_cast<Code::Word>(ObjectMember::MAX);

      if(auto dot = static_cast<char const *>(std::memchr(glyph.str, '.', glyph.len)))
      {
         if(auto type = Code::ExtMems.findVal({glyph.begin(), dot}))
            if(auto mem = type->find({dot + 1, glyph.end()}))
               return static_cast<Code::Word>(ObjectMember::MAX) + *mem;
      }
      else if(auto type = Code::ExtMems.findVal(glyph))
         return static_cast<Code::Word>(ObjectMember::MAX) + type->max();

      throw Code::GlyphError{"ObjectMemberExt", glyph};
   }

   //
   // {ObjectType}
   //
   DGE_Code_GlyphTypeDefn(ObjectType)
   {
      if(auto type = ObjectTypes.findItr(glyph))
         return static_cast<Code::Word>(type - ObjectTypes.begin());

      throw Code::GlyphError{"ObjectType", glyph};
   }
}


//----------------------------------------------------------------------------|
// Natives                                                                    |
//

namespace DGE::Game
{
   //
   // unsigned DGE_Object_Cast(unsigned id, unsigned type)
   //
   DGE_Code_NativeDefn(DGE_Object_Cast)
   {
      task->dataStk.push(ObjectTypes[argv[1]](Object::Get(argv[0])));
      return false;
   }

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
   DGE_Game_Object_MemberGetDefn(LR)
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
   DGE_Game_Object_MemberSetDefn(LR)
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

