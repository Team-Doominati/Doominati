//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Module-specific *DEFS handling.
//
//-----------------------------------------------------------------------------

#include "GL/Defs.hpp"

#include "Code/Native.hpp"

#include "Defs/Gamedefs.hpp"


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::GL
{
   DGE_Code_NativeLoaderDecl(Renderer_Draw);
   DGE_Code_NativeLoaderDecl(Renderer_DrawText);
   DGE_Code_NativeLoaderDecl(Renderer_Font);

   //
   // GetDefs
   //
   Defs::GamedefsGroup &GetDefs()
   {
      static Defs::GamedefsGroup defs{&Defs::Gamedefs::GetRoot(), "GL"};
      return defs;
   }

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

