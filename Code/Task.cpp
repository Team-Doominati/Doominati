//-----------------------------------------------------------------------------
//
// Copyright (C) 2016 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Script VM tasks.
//
//-----------------------------------------------------------------------------

#include "Code/Task.hpp"

#include "Code/Thread.hpp"

#include <iostream>
#include <algorithm>


//----------------------------------------------------------------------------|
// Static Objects                                                             |
//

namespace DGE::Code
{
   static Core::ListLink<Task> TaskFree;
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Code
{
   //
   // Task constructor
   //
   Task::Task() :
      link{this},
      codePtr{nullptr},
      delay{0}
   {
      callStk.reserve(CallStkSize);
      dataStk.reserve(DataStkSize);
   }

   //
   // Task::stop
   //
   void Task::stop()
   {
      // Release execution resources.
      callStk.clear();
      dataStk.clear();
      locReg.clear();

      // Move to free list.
      link.relink(&TaskFree);
   }

   //
   // Task::stop
   //
   Word Task::stop(Word *retV, Word retC)
   {
      Word stkC = std::min<Word>(retC, dataStk.size());

      std::copy(dataStk.data(), dataStk.data() + stkC, retV);
      std::fill(retV + stkC, retV + retC, 0);

      stop();

      return stkC;
   }

   //
   // Task::Create
   //
   Task *Task::Create(Thread *thrd)
   {
      Task *task;
      if(TaskFree.next->obj)
      {
         task = TaskFree.next->obj;
         task->link.unlink();
      }
      else
         task = new Task;

      task->prog = thrd->prog;
      task->proc = thrd->proc;
      task->thrd = thrd;

      task->link.insert(&thrd->tasks);

      task->delay = 0;

      return task;
   }
}

// EOF

