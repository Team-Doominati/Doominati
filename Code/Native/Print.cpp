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
   DGE_Code_NativeLoaderDefn(Native_Print);

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
}

// EOF

