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
      Callbacks[argv[0]]->insert(task->prog, argv[1]);
      return false;
   }

   //
   // DGE_Callback_Unregister(unsigned cb, void (*fn)(void))
   //
   DGE_Code_NativeDefn(DGE_Callback_Unregister)
   {
      Callbacks[argv[0]]->erase(task->prog, argv[1]);
      return false;
   }
}

// EOF

