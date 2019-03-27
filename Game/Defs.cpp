//-----------------------------------------------------------------------------
//
// Copyright (C) 2017-2019 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Module-specific *DEFS handling.
//
//-----------------------------------------------------------------------------

#include "Game/Defs.hpp"

#include "Code/Native.hpp"

#include "Defs/Gamedefs.hpp"


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
   // GetDefs
   //
   Defs::GamedefsGroup &GetDefs()
   {
      static Defs::GamedefsGroup defs{&Defs::Gamedefs::GetRoot(), "Game"};
      return defs;
   }

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

