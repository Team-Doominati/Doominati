//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Scripting native adder.
//
//-----------------------------------------------------------------------------

#include "Code/Native.hpp"


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Game
{
   void MissileEntity_NativeAdd();
   void PointThinker_NativeAdd();
   void Sector_NativeAdd();

   //
   // NativeAdd
   //
   void NativeAdd()
   {
      MissileEntity_NativeAdd();
      PointThinker_NativeAdd();
      Sector_NativeAdd();
   }
}

// EOF

