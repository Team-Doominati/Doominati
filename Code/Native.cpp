//-----------------------------------------------------------------------------
//
// Copyright (C) 2016 Team Doominati
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


//----------------------------------------------------------------------------|
// Static Functions                                                           |
//

namespace Doom
{
   namespace Code
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
}


//----------------------------------------------------------------------------|
// Extern Objects                                                             |
//

namespace Doom
{
   namespace Code
   {
      Core::HashMapFixed<Core::HashedStr, Native> Natives;
   }
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace Doom
{
   namespace Code
   {
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
}

// EOF

