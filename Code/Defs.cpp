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

#include "Code/Defs.hpp"

#include "Defs/Gamedefs.hpp"


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Code
{
   //
   // GetDefs
   //
   Defs::GamedefsGroup &GetDefs()
   {
      static Defs::GamedefsGroup defs{&Defs::Gamedefs::GetRoot(), "Code"};
      return defs;
   }
}

// EOF

