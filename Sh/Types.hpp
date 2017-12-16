//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Common typedefs.
//
//-----------------------------------------------------------------------------

#ifndef DGE__Sh__Types_H__
#define DGE__Sh__Types_H__

#include "../Code/Types.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::Sh
{
   class Shell;

   using Command = int (*)(Shell *, char **, std::size_t);
}

#endif//DGE__Sh__Types_H__

