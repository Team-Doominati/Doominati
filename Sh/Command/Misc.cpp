//-----------------------------------------------------------------------------
//
// Copyright (C) 2017-2019 Team Doominati
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

#include "Code/Callback.hpp"
#include "Code/Native.hpp"
#include "Code/Textdefs.hpp"

#include <algorithm>
#include <iostream>
#include <vector>


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

   //
   // help
   //
   DGE_Sh_CommandDefn(help)
   {
      DGE_Sh_OptionsDecl("help");
      DGE_Sh_OptionsProcess();

      std::vector<Core::HashedStr> cmds;

      if(opts.args.size() == 0)
      {
         for(auto const &cmd : Commands)
            cmds.emplace_back(cmd.key);

         for(auto const &cmd : VMCommands)
            cmds.emplace_back(cmd.first);
      }
      else if(opts.args.size() == 1)
      {
         auto name = opts.args.data()[0];
         if(Commands.find(name) || VMCommands.find(name))
         {
            cmds.emplace_back(name);
         }
         else
         {
            auto nameEnd = name + std::strlen(name);

            for(auto const &cmd : Commands)
               if(std::search(cmd.key.begin(), cmd.key.end(), name, nameEnd) != cmd.key.end())
                  cmds.emplace_back(cmd.key);

            for(auto const &cmd : VMCommands)
               if(std::search(cmd.first.begin(), cmd.first.end(), name, nameEnd) != cmd.first.end())
                  cmds.emplace_back(cmd.first);
         }
      }
      else
      {
         std::cout << "usage: help [command]\n";
         return 1;
      }

      if(cmds.size() == 1)
      {
         std::string buf{cmds.front()};
         buf += ".desc";
         if(Core::HashedStr lang = Code::Textdefs.find({buf.data(), buf.size()}))
            std::cout << cmds.front() << ": " << lang << "\n";
         else
            std::cout << cmds.front() << ": <missing description>\n";
      }
      else
      {
         std::sort(cmds.begin(), cmds.end());

         for(auto const &cmd : cmds)
            std::cout << cmd << "\n";
      }

      return 0;
   }
}

// EOF

