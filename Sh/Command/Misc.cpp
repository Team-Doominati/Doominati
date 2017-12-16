//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Miscellaneous commands.
//
//-----------------------------------------------------------------------------

#include "Sh/Command.hpp"

#include "Sh/Option.hpp"

#include "Code/Native.hpp"

#include <iostream>


//----------------------------------------------------------------------------|
// Natives                                                                    |
//

namespace DGE::Sh
{
   DGE_Code_NativeLoaderDefn(Command_Misc);

   //
   // echo
   //
   DGE_Sh_CommandDefn(echo)
   {
      DGE_Sh_OptionsDecl("echo");
      DGE_Sh_OptionsProcess();

      for(auto arg : opts.args)
         std::cout << arg << ' ';
      std::cout << std::endl;

      return 0;
   }

   //
   // exit
   //
   DGE_Sh_CommandDefn(exit)
   {
      DGE_Sh_OptionsDecl("exit");
      DGE_Sh_OptionsProcess();

      throw EXIT_SUCCESS;
   }
}

// EOF

