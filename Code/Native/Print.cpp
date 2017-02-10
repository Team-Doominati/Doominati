//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Printing natives.
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
   //
   // void DGE_PrintChar(unsigned c)
   //
   DGE_Code_NativeDefn(DGE_PrintChar)
   {
      std::cout.put(argv[0]);
      return false;
   }

   //
   // void DGE_PrintWordD(unsigned d)
   //
   DGE_Code_NativeDefn(DGE_PrintWordD)
   {
      std::cout << argv[0];
      return false;
   }

   //
   // SysWrite
   //
   static Word SysWrite(std::ostream &out, MemPtr<Byte const> buf, Word len)
   {
      for(auto itr = buf, end = buf + len; itr != end; ++itr)
         out.put(*itr);

      return len;
   }

   //
   // unsigned DGE_SysWrite(char const *buf, unsigned len)
   //
   DGE_Code_NativeDefn(DGE_SysWrite)
   {
      task->dataStk.push(SysWrite(std::cout, {&task->prog->memory, argv[0]}, argv[1]));
      return false;
   }

   //
   // unsigned DGE_SysWriteErr(char const *buf, unsigned len)
   //
   DGE_Code_NativeDefn(DGE_SysWriteErr)
   {
      task->dataStk.push(SysWrite(std::cerr, {&task->prog->memory, argv[0]}, argv[1]));
      return false;
   }
}

// EOF

