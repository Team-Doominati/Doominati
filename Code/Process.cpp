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
      (new Thread(this))->link.insert(&threads);
   }

   //
   // Process destructor
   //
   Process::~Process()
   {
      while(threads.next->obj)
         delete threads.next->obj;
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
}

// EOF

