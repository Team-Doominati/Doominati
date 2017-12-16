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

#include "Sh/Option.hpp"

#include <iostream>


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Sh
{
   //
   // Options constructor
   //
   Options::Options() :
      prog{},

      args{nullptr, GDCC::Option::Base::Info(), 0},

      optHelp
      {
         &prog, GDCC::Option::Base::Info()
            .setName("help").setName('h')
            .setDescS("Prints usage and exits."),

         [](GDCC::Option::Base *opt, GDCC::Option::Args const &) -> std::size_t
            {opt->getProgram()->putHelp(std::cout); throw EXIT_SUCCESS;}
      },

      optHelpLong
      {
         &prog, GDCC::Option::Base::Info()
            .setName("help-long")
            .setDescS("Prints full usage and exits."),

         [](GDCC::Option::Base *opt, GDCC::Option::Args const &) -> std::size_t
            {opt->getProgram()->putHelpLong(std::cout); throw EXIT_SUCCESS;}
      }
   {
      prog.processLoose = &args;
   }
}

// EOF

