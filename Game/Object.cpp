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

#include "Code/Native.hpp"


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
// Natives                                                                    |
//

namespace DGE::Game
{
   //
   // void DGE_ObjectRefAdd(unsigned id)
   //
   DGE_Code_NativeDefn(DGE_ObjectRefAdd)
   {
      if(auto th = Object::Get(argv[0])) th->refAdd();
      return false;
   }

   //
   // void DGE_ObjectRefSub(unsigned id)
   //
   DGE_Code_NativeDefn(DGE_ObjectRefSub)
   {
      if(auto th = Object::Get(argv[0])) th->refSub();
      return false;
   }
}

// EOF

