//-----------------------------------------------------------------------------
//
// Copyright (C) 2016 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Script VM threads.
//
//-----------------------------------------------------------------------------

#ifndef DGE__Code__Thread_H__
#define DGE__Code__Thread_H__

#include "Code/Types.hpp"

#include "Core/List.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::Code
{
   //
   // Thread
   //
   class Thread
   {
   public:
      Thread(Process *proc);
      ~Thread();

      void exec();

      Task *startTask(Function *func, Word const *argV, Word argC);

      Core::ListLink<Thread> link;
      Core::ListLink<Task>   tasks;

      Process *const proc;
      Program *const prog;
   };
}

#endif//DGE__Code__Thread_H__

