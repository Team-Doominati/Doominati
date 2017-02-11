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
   // Callback.cpp
   DGE_Code_NativeDecl(DGE_CallbackRegister);
   DGE_Code_NativeDecl(DGE_CallbackUnregister);

   // Native/Debug.cpp
   DGE_Code_NativeDecl(DGE_DebugCallStk);
   DGE_Code_NativeDecl(DGE_DebugDataStk);
   DGE_Code_NativeDecl(DGE_DebugLocReg);

   // Native/File.cpp
   DGE_Code_NativeDecl(DGE_FileClose);
   DGE_Code_NativeDecl(DGE_FileGet);
   DGE_Code_NativeDecl(DGE_FileOpen);
   DGE_Code_NativeDecl(DGE_FileRead);
   DGE_Code_NativeDecl(DGE_FileSize);

   // Native/Memory.cpp
   DGE_Code_NativeDecl(DGE_FreestoreBegin);
   DGE_Code_NativeDecl(DGE_FreestoreEnd);

   // Native/Print.cpp
   DGE_Code_NativeDecl(DGE_PrintChar);
   DGE_Code_NativeDecl(DGE_PrintWordD);
   DGE_Code_NativeDecl(DGE_SysWrite);
   DGE_Code_NativeDecl(DGE_SysWriteErr);

   // Native/State.cpp
   DGE_Code_NativeDecl(DGE_Delay);

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
      // Callback.cpp
      DGE_Code_NativeAdd(DGE_CallbackRegister);
      DGE_Code_NativeAdd(DGE_CallbackUnregister);

      // Native/Debug.cpp
      DGE_Code_NativeAdd(DGE_DebugCallStk);
      DGE_Code_NativeAdd(DGE_DebugDataStk);
      DGE_Code_NativeAdd(DGE_DebugLocReg);

      // Native/File.cpp
      DGE_Code_NativeAdd(DGE_FileClose);
      DGE_Code_NativeAdd(DGE_FileGet);
      DGE_Code_NativeAdd(DGE_FileOpen);
      DGE_Code_NativeAdd(DGE_FileRead);
      DGE_Code_NativeAdd(DGE_FileSize);

      // Native/Memory.cpp
      DGE_Code_NativeAdd(DGE_FreestoreBegin);
      DGE_Code_NativeAdd(DGE_FreestoreEnd);

      // Native/Print.cpp
      DGE_Code_NativeAdd(DGE_PrintChar);
      DGE_Code_NativeAdd(DGE_PrintWordD);
      DGE_Code_NativeAdd(DGE_SysWrite);
      DGE_Code_NativeAdd(DGE_SysWriteErr);

      // Native/State.cpp
      DGE_Code_NativeAdd(DGE_Delay);

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

