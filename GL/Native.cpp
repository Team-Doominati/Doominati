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

namespace DGE::GL
{
   DGE_Code_NativeLoaderDecl(Renderer_Draw);
   DGE_Code_NativeLoaderDecl(Renderer_DrawText);
   DGE_Code_NativeLoaderDecl(Renderer_Font);

   //
   // NativeAdd
   //
   void NativeAdd()
   {
      DGE_Code_NativeLoaderCall(Renderer_Draw);
      DGE_Code_NativeLoaderCall(Renderer_DrawText);
      DGE_Code_NativeLoaderCall(Renderer_Font);
   }
}

// EOF

