//-----------------------------------------------------------------------------
//
// Copyright (C) 2017-2019 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Common typedefs.
//
//-----------------------------------------------------------------------------

#ifndef DGE__GL__Types_H__
#define DGE__GL__Types_H__

#include "../GL/OpenGL2.1.h"

#include "../Code/Types.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::GL
{
   class FontFace;
   class TextureData;
   class Window;

   using TexturePixel = GLfloat[4];
   using TextureDim   = GLsizei;
}

#endif//DGE__GL__Types_H__

