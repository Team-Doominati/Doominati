//-----------------------------------------------------------------------------
//
// Copyright (C) 2016 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Script VM processes.
//
//-----------------------------------------------------------------------------

#include "Code/Process.hpp"

#include "Code/Task.hpp"
#include "Code/Thread.hpp"


//----------------------------------------------------------------------------|
// Extern Objects                                                             |
//

namespace DGE::Code
{
   Process *Process::Main = nullptr;
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Code
{
   //
   // Process constructor
   //
   Process::Process(Program *prog_) :
      prog{prog_}
   {
      if(!Main) Main = this;
   }

   //
   // Process destructor
   //
   Process::~Process()
   {
      if(Main == this)
         Main = nullptr;

      while(threads.next->obj)
         delete threads.next->obj;
   }

   //
   // Process::call
   //
   Word Process::call(Function *func, Word const *argV, Word argC, Word *retV, Word retC)
   {
      Task *task = mainThread()->startTask(func, argV, argC);
      task->exec();
      return task->stop(retV, retC);
   }

   //
   // Process::exec
   //
   void Process::exec()
   {
      // TODO: Hardware threads.

      for(auto &thread : threads)
         thread.exec();
   }

   //
   // Process::mainThread
   //
   Thread *Process::mainThread()
   {
      if(threads.empty())
         (new Thread(this))->link.insert(&threads);

      return threads.next->obj;
   }
}

// EOF

