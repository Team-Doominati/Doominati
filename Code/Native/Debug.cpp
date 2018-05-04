//-----------------------------------------------------------------------------
//
// Copyright (C) 2017-2018 Team Doominati
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

namespace DGE::Code
{
   DGE_Code_NativeLoaderDefn(Native_Debug);

   //
   // void DGE_DebugCallStk(void)
   //
   DGE_Code_NativeDefn(DGE_DebugCallStk)
   {
      PrintCallStack(std::cerr, task);
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
      std::cerr << &task->locReg[0] << "+" << task->locReg.size() << '+' << task->vaaRegC << '\n';

      // Print variadic register contents.
      for(auto end = task->locReg.begin(), itr = end - task->vaaRegC; itr != end; ++itr)
         std::cerr << "  ..." << *itr << '\n';

      // Print normal register contents.
      for(auto const &reg : task->locReg)
         std::cerr << "  " << reg << '\n';

      return false;
   }
}

// EOF

