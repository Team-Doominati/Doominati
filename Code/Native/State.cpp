//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Execution state control functions.
//
//-----------------------------------------------------------------------------

#include "Code/Native.hpp"

#include "Code/Task.hpp"


//----------------------------------------------------------------------------|
// Natives                                                                    |
//

namespace DGE::Code
{
   //
   // void DGE_Delay(unsigned ticks)
   //
   DGE_Code_NativeDefn(DGE_Delay)
   {
      task->delay = argv[0];
      return true;
   }
}

// EOF

