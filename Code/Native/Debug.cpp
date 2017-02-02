//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Debugging natives.
//
//-----------------------------------------------------------------------------

#include "Code/Native.hpp"

#include "Code/MemPtr.hpp"
#include "Code/Program.hpp"
#include "Code/Task.hpp"

#include <iostream>


//----------------------------------------------------------------------------|
// Natives                                                                    |
//

DGE_Code_NativeLoader(Debug)

namespace DGE::Code
{
   //
   // PrintCallFrame
   //
   void PrintCallFrame(std::ostream &out, CallFrame const &call, Task const *task)
   {
      auto orig = task->prog->getOrigin(call.codePtr);

      out << "  " << orig.func;
      if(orig.file)
         out << " (" << orig.file << ")";
      out << " [" << call.locRegC << '/' << call.vaaRegC<< "]\n";
   }

   //
   // void DGE_DebugCallStk(void)
   //
   DGE_Code_NativeDefn(DGE_DebugCallStk)
   {
      std::cerr << task->callStk.data() << '+' << task->callStk.size() << '\n';
      PrintCallFrame(std::cerr,
         {task->codePtr, (Word)task->locReg.size(), task->vaaRegC}, task);
      for(auto i = task->callStk.rbegin(), e = task->callStk.rend(); i != e;)
         PrintCallFrame(std::cerr, *i++, task);
      return false;
   }

   //
   // void DGE_DebugDataStk(int dump)
   //
   DGE_Code_NativeDefn(DGE_DebugDataStk)
   {
      bool dump = argc < 1 ? 0 : argv[0] & 1;

      std::cerr << task->dataStk.data() << '+' << task->dataStk.size() << '\n';
      if(dump) for(auto const &val : task->dataStk)
         std::cerr << "  " << val << '\n';
      return false;
   }

   //
   // void DGE_DebugLocReg(void)
   //
   DGE_Code_NativeDefn(DGE_DebugLocReg)
   {
      std::cerr << &task->locReg[0] << "+" << task->locReg.size() << '\n';
      for(auto const &reg : task->locReg)
         std::cerr << "  " << reg << '\n';
      return false;
   }
}

// EOF

