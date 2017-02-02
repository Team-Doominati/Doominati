//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Script VM native functions.
//
//-----------------------------------------------------------------------------

#include "Native.hpp"

#include <utility>
#include <vector>

#include <iostream>


//----------------------------------------------------------------------------|
// Static Functions                                                           |
//

namespace DGE::Code
{
   //
   // NativeVec
   //
   static std::vector<std::pair<Core::HashedStr, Native>> *&NativeVec()
   {
      static auto *vec = new std::vector<std::pair<Core::HashedStr, Native>>;
      return vec;
   }
}


//----------------------------------------------------------------------------|
// Extern Objects                                                             |
//

namespace DGE::Code
{
   Core::HashMapFixed<Core::HashedStr, Native> Natives;
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Code
{
   void NativeLoad_Debug();
   void NativeLoad_DrawText();
   void NativeLoad_File();
   void NativeLoad_Memory();
   void NativeLoad_Print();
   void NativeLoad_State();

   //
   // NativeAdder constructor
   //
   NativeAdder::NativeAdder(Core::HashedStr name, Native native)
   {
      NativeVec()->push_back({name, native});
   }

   //
   // NativeAdder::Finish
   //
   void NativeAdder::Finish()
   {
      // To ensure that the static objects of a compilation unit in a static
      // library are actually constructed, a function or object from that
      // compilation unit must be used.
      // Other solutions exist, but are overly vendor-specific.
      NativeLoad_Debug();
      NativeLoad_DrawText();
      NativeLoad_File();
      NativeLoad_Memory();
      NativeLoad_Print();
      NativeLoad_State();

      auto &vec = NativeVec();
      Natives.alloc(vec->size());

      auto vecItr = vec->begin();
      for(auto &native : Natives)
      {
         native.key = vecItr->first;
         native.val = vecItr->second;
         ++vecItr;
      }

      Natives.build();
      delete vec;
      vec = nullptr;
   }
}

// EOF

