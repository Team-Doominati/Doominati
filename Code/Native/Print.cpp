//-----------------------------------------------------------------------------
//
// Copyright (C) 2016 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Printing natives.
//
//-----------------------------------------------------------------------------

#include "Code/Native.hpp"

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
}

// EOF

