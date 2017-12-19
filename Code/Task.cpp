//-----------------------------------------------------------------------------
//
// Copyright (C) 2016-2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Script VM tasks.
//
//-----------------------------------------------------------------------------

#include "Code/Task.hpp"

#include "Code/Native.hpp"
#include "Code/Program.hpp"
#include "Code/Thread.hpp"

#include <algorithm>
#include <memory>


//----------------------------------------------------------------------------|
// Static Objects                                                             |
//

namespace DGE::Code
{
   static Core::ListHead<Task> TaskFree;
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
      link   {this},
      codePtr{nullptr},
      delay  {0},
      id     {0}, // TODO: Allocate ids.
      jumpbuf{0}
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
      link.relink(&TaskFree.head);
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
      if(TaskFree.head.next->obj)
      {
         task = TaskFree.head.next->obj;
         task->link.unlink();
      }
      else
         task = new Task;

      task->prog = thrd->prog;
      task->proc = thrd->proc;
      task->thrd = thrd;

      task->link.insert(&thrd->tasks);

      task->delay   = 0;
      task->join    = 0;
      task->jumpbuf = 0;

      return task;
   }
}


//----------------------------------------------------------------------------|
// Natives                                                                    |
//

namespace DGE::Code
{
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

   //
   // void DGE_Task_Sleep(unsigned id, unsigned ticks)
   //
   DGE_Code_NativeDefn(DGE_Task_Sleep)
   {
      // TODO: Lookup task by id.
      auto t = argv[0] ? nullptr : task;

      if(t)
         t->delay = argv[1];

      return t == task;
   }
}

// EOF

