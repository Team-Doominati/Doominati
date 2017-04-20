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
   Core::IDAllocator<Object, Code::Word> Object::ObjectVec;
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
      id{ObjectVec.alloc(this)}
   {
   }

   //
   // Object destructor
   //
   Object::~Object()
   {
      ObjectVec.free(id);
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

