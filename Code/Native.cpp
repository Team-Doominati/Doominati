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
   void NativeAdder::Add(Core::HashedStr name, Native native)
   {
      NativeVec()->push_back({name, native});
   }

   //
   // NativeAdder::Finish
   //
   void NativeAdder::Finish()
   {
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

      auto &vec = NativeVec();

      auto vecItr = vec->begin();
      Natives.reset(vec->size(),
         [&](auto elem){elem->key = vecItr->first; elem->val = vecItr->second; ++vecItr;});

      delete vec;
      vec = nullptr;
   }
}

// EOF

