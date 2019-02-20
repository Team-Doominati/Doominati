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

#include "FS/Dir.hpp"


//----------------------------------------------------------------------------|
// Static Objects                                                             |
//

namespace DGE::Code
{
   static Process *MainProcess;
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
      prog{prog_},

      workDir{nullptr}
   {
      if(!MainProcess) MainProcess = this;
   }

   //
   // Process destructor
   //
   Process::~Process()
   {
      if(MainProcess == this)
         MainProcess = nullptr;

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

   FS::Dir::DirPtr Process::getWorkDir()
   {
      if(auto dir = FS::Dir::Root->findDirPath(workDir))
         return dir;

      return {FS::Dir::Root.get(), false};
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

   //
   // Process::setWorkDir
   //
   void Process::setWorkDir(std::unique_ptr<char[]> &&str, std::size_t len)
   {
      workDirData = std::move(str);
      workDir     = {workDirData.get(), len};
   }

   //
   // Process::GetMain
   //
   Process *Process::GetMain()
   {
      return MainProcess;
   }
}

// EOF

