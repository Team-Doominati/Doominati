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

#ifndef DGE__Game__Defs_H__
#define DGE__Game__Defs_H__

#include "../Game/Types.hpp"


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Game
{
   Defs::GamedefsGroup &GetDefs();
   void NativeAdd();
}

#endif//DGE__Game__Defs_H__

