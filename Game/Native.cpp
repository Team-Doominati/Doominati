//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Scripting native adder.
//
//-----------------------------------------------------------------------------

#include "Code/Native.hpp"


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Game
{
   // Input.cpp
   DGE_Code_NativeDecl(DGE_GetInputAxis);
   DGE_Code_NativeDecl(DGE_GetInputButtons);

   //
   // NativeAdd
   //
   void NativeAdd()
   {
      // Input.cpp
      DGE_Code_NativeAdd(DGE_GetInputAxis);
      DGE_Code_NativeAdd(DGE_GetInputButtons);
   }
}

// EOF

