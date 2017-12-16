//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Command option handling.
//
//-----------------------------------------------------------------------------

#ifndef DGE__Sh__Option_H__
#define DGE__Sh__Option_H__

#include "../Sh/Types.hpp"

#include <GDCC/Option/CStrV.hpp>
#include <GDCC/Option/Function.hpp>
#include <GDCC/Option/Program.hpp>


//----------------------------------------------------------------------------|
// Macros                                                                     |
//

//
// DGE_Sh_OptionsDecl
//
#define DGE_Sh_OptionsDecl(progname) \
   ::DGE::Sh::Options opts; \
   opts.prog.name = (progname) \

//
// DGE_Sh_OptionsProcess
//
#define DGE_Sh_OptionsProcess() \
   try \
   { \
      opts.prog.process(GDCC::Option::Args() \
        .setArgs(argv + 1, argc - 1) \
        .setOptKeepA()); \
   } \
   catch(int e) \
   { \
      return e; \
   }


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::Sh
{
   //
   // Options
   //
   class Options
   {
   public:
      Options();

      GDCC::Option::Program prog;

      GDCC::Option::CStrV args;

      GDCC::Option::Function optHelp;
      GDCC::Option::Function optHelpLong;
   };
}

#endif//DGE__Sh__Option_H__

