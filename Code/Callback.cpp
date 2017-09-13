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

#include "Code/Glyph.hpp"
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
// Glyph Types                                                                |
//

namespace DGE::Code
{
   //
   // {Callback}
   //
   DGE_Code_GlyphTypeDefn(Callback)
   {
      if(auto type = Callbacks.findItr(glyph))
         return static_cast<Code::Word>(type - Callbacks.begin());

      throw Code::GlyphError{"Callback", glyph};
   }
}


//----------------------------------------------------------------------------|
// Natives                                                                    |
//

namespace DGE::Code
{
   //
   // DGE_Callback_Register(unsigned cb, void (*fn)(void))
   //
   DGE_Code_NativeDefn(DGE_Callback_Register)
   {
      Callbacks[argv[0]]->insert(&Code::Process::GetMain()->prog->funcs[argv[1]]);
      return false;
   }

   //
   // DGE_Callback_Unregister(unsigned cb, void (*fn)(void))
   //
   DGE_Code_NativeDefn(DGE_Callback_Unregister)
   {
      Callbacks[argv[0]]->erase(&Code::Process::GetMain()->prog->funcs[argv[1]]);
      return false;
   }
}

// EOF

