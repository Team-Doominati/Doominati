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


//----------------------------------------------------------------------------|
// Static Functions                                                           |
//

namespace DGE::Code
{
   //
   // CallbackVec
   //
   static auto &CallbackVec()
   {
      static auto *vec = new std::vector<std::pair<Core::HashedStr, Callback *>>;
      return vec;
   }

   //
   // NativeVec
   //
   static auto &NativeVec()
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
   Core::HashMapFixed<Core::HashedStr, Callback *> Callbacks;
   Core::HashMapFixed<Core::HashedStr, Native> Natives;
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Code
{
   DGE_Code_NativeLoaderDecl(Native_Debug);
   DGE_Code_NativeLoaderDecl(Native_File);
   DGE_Code_NativeLoaderDecl(Native_Memory);
   DGE_Code_NativeLoaderDecl(Native_Print);
   DGE_Code_NativeLoaderDecl(Native_String);

   //
   // NativeAdder constructor
   //
   NativeAdder::NativeAdder(Core::HashedStr name, Native native)
   {
      Add(name, native);
   }

   //
   // NativeAdder::Add
   //
   void NativeAdder::Add(Core::HashedStr name, Callback *cb)
   {
      CallbackVec()->push_back({name, cb});
   }

   //
   // NativeAdder::Add
   //
   void NativeAdder::Add(Core::HashedStr name, Native native)
   {
      NativeVec()->push_back({name, native});
   }

   //
   // NativeAdder::Finish
   //
   void NativeAdder::Finish()
   {
      DGE_Code_NativeLoaderCall(Native_Debug);
      DGE_Code_NativeLoaderCall(Native_File);
      DGE_Code_NativeLoaderCall(Native_Memory);
      DGE_Code_NativeLoaderCall(Native_Print);
      DGE_Code_NativeLoaderCall(Native_String);

      // Callbacks.
      {
         auto &vec = CallbackVec();

         auto vecItr = vec->begin();
         Callbacks.reset(vec->size(),
            [&](auto elem){elem->key = vecItr->first; elem->val = vecItr->second; ++vecItr;});

         delete vec;
         vec = nullptr;
      }

      // Natives.
      {
         auto &vec = NativeVec();

         auto vecItr = vec->begin();
         Natives.reset(vec->size(),
            [&](auto elem){elem->key = vecItr->first; elem->val = vecItr->second; ++vecItr;});

         delete vec;
         vec = nullptr;
      }
   }
}

// EOF

