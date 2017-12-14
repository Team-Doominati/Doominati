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

#ifndef DGE__GL__Defs_H__
#define DGE__GL__Defs_H__

#include "../GL/Types.hpp"


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::GL
{
   Defs::GamedefsGroup &GetDefs();
   void NativeAdd();
}

#endif//DGE__GL__Defs_H__

