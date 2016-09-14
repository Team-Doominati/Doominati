//-----------------------------------------------------------------------------
//
// Copyright (C) 2016 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Memory access/control functions.
//
//-----------------------------------------------------------------------------

#include "Code/Native.hpp"

#include "Code/Program.hpp"
#include "Code/Task.hpp"


//----------------------------------------------------------------------------|
// Natives                                                                    |
//

namespace Doom
{
   namespace Code
   {
      //
      // void *DGE_FreestoreBegin(void)
      //
      Doom_Code_NativeDefn(DGE_FreestoreBegin)
      {
         task->dataStk.push(task->prog->freestoreBegin);
         return false;
      }

      //
      // void *DGE_FreestoreEnd(void)
      //
      Doom_Code_NativeDefn(DGE_FreestoreEnd)
      {
         task->dataStk.push(task->prog->freestoreEnd);
         return false;
      }
   }
}

// EOF

