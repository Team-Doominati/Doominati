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
   DGE_Code_NativeLoaderDecl(Inventory);
   DGE_Code_NativeLoaderDecl(MissileEntity);
   DGE_Code_NativeLoaderDecl(ParticleSys);
   DGE_Code_NativeLoaderDecl(Sector);
   DGE_Code_NativeLoaderDecl(ScriptedEntity);

   //
   // NativeAdd
   //
   void NativeAdd()
   {
      DGE_Code_NativeLoaderCall(Inventory);
      DGE_Code_NativeLoaderCall(MissileEntity);
      DGE_Code_NativeLoaderCall(ParticleSys);
      DGE_Code_NativeLoaderCall(Sector);
      DGE_Code_NativeLoaderCall(ScriptedEntity);
   }
}

// EOF

