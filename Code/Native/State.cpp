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

#include "Code/Program.hpp"
#include "Code/Task.hpp"
#include "Code/Thread.hpp"


//----------------------------------------------------------------------------|
// Natives                                                                    |
//

namespace DGE::Code
{
   DGE_Code_NativeLoaderDefn(Native_State);

   //
   // void DGE_Delay(unsigned ticks)
   //
   DGE_Code_NativeDefn(DGE_Delay)
   {
      task->delay = argv[0];
      return true;
   }

   //
   // unsigned DGE_Task_Create(unsigned thread, void (*fn)(void), ...)
   //
   DGE_Code_NativeDefn(DGE_Task_Create)
   {
      // TODO: Lookup thread by id.
      auto thrd = argv[0] ? nullptr : task->thrd;
      auto fn   = &task->prog->funcs[argv[1]];

      if(thrd)
         task->dataStk.push(thrd->startTask(fn, argv + 2, argc - 2)->id);
      else
         task->dataStk.push(0);

      return false;
   }
}

// EOF

