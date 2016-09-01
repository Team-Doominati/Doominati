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

#ifndef Doom__Code__Thread_H__
#define Doom__Code__Thread_H__

#include "Code/Types.hpp"

#include "Core/List.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace Doom
{
   namespace Code
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

         void startTask(Function *func, Word const *argV, Word argC);

         Core::ListLink<Thread> link;
         Core::ListLink<Task>   tasks;

         Process *const proc;
         Program *const prog;
      };
   }
}

#endif//Doom__Code__Thread_H__

