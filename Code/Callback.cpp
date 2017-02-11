//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Script VM registerable callback handling.
//
//-----------------------------------------------------------------------------

#include "Code/Callback.hpp"

#include "Code/MemStr.hpp"
#include "Code/Native.hpp"
#include "Code/Process.hpp"
#include "Code/Program.hpp"
#include "Code/Task.hpp"


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Code
{
   //
   // Callback constructor
   //
   Callback::Callback(Core::HashedStr name)
   {
      NativeAdder::Add(name, this);
   }

   //
   // Callback::call
   //
   void Callback::call(Word const *argV, Word argC)
   {
      auto proc = Process::GetMain();
      for(auto func : funcs)
         proc->call(func, argV, argC, nullptr, 0);
   }

   //
   // Callback::erase
   //
   void Callback::erase(Function *func)
   {
      for(auto itr = funcs.begin(), end = funcs.end(); itr != end;)
      {
         if(*itr == func)
            itr = funcs.erase(itr);
         else
            ++itr;
      }
   }

   //
   // Callback::insert
   //
   void Callback::insert(Function *func)
   {
      for(auto f : funcs)
         if(f == func) return;

      funcs.push_back(func);
   }
}


//----------------------------------------------------------------------------|
// Natives                                                                    |
//

namespace DGE::Code
{
   //
   // DGE_CallbackRegister(char const *name, void (*callback)(void))
   //
   DGE_Code_NativeDefn(DGE_CallbackRegister)
   {
      auto name = MemStrDup({&task->prog->memory, argv[0]});
      auto func = &Code::Process::GetMain()->prog->funcs[argv[1]];

      if(auto cb = Callbacks.find(name.get()))
         (*cb)->insert(func);

      return false;
   }

   //
   // DGE_CallbackUnregister(char const *name, void (*callback)(void))
   //
   DGE_Code_NativeDefn(DGE_CallbackUnregister)
   {
      auto name = MemStrDup({&task->prog->memory, argv[0]});
      auto func = &Code::Process::GetMain()->prog->funcs[argv[1]];

      if(auto cb = Callbacks.find(name.get()))
         (*cb)->erase(func);

      return false;
   }
}

// EOF

