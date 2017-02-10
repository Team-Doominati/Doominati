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
   //
   // NativeAdd
   //
   void NativeAdd()
   {
      // Renderer.cpp
      DGE_Code_NativeAdd(DGE_CallbackDrawBegin);
      DGE_Code_NativeAdd(DGE_CallbackDrawEnd);
      DGE_Code_NativeAdd(DGE_CallbackResize);
      DGE_Code_NativeAdd(DGE_SetVirtualResolution);

      // Renderer/Draw.cpp
      DGE_Code_NativeAdd(DGE_DrawColor);
      DGE_Code_NativeAdd(DGE_DrawCircle);
      DGE_Code_NativeAdd(DGE_DrawCircleLine);
      DGE_Code_NativeAdd(DGE_DrawEllipse);
      DGE_Code_NativeAdd(DGE_DrawEllipseLine);
      DGE_Code_NativeAdd(DGE_DrawRectangle);
      DGE_Code_NativeAdd(DGE_DrawRectangleLine);
      DGE_Code_NativeAdd(DGE_DrawLine);
      DGE_Code_NativeAdd(DGE_DrawTexture);
      DGE_Code_NativeAdd(DGE_DrawTriangle);
      DGE_Code_NativeAdd(DGE_DrawTriangleLine);
      DGE_Code_NativeAdd(DGE_GetTexture);

      // Renderer/DrawText.cpp
      DGE_Code_NativeAdd(DGE_DrawText);
      DGE_Code_NativeAdd(DGE_TextAlignment);
   }
}

// EOF

