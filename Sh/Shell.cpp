//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Builtin console shell.
//
//-----------------------------------------------------------------------------

#include "Sh/Shell.hpp"

#include "Sh/Command.hpp"
#include "Sh/StdIn.hpp"

#include "Core/HashMapFixed.hpp"
#include "Core/String.hpp"

#include <iostream>


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Sh
{
   //
   // Shell constructor
   //
   Shell::Shell() :
      state{&Shell::execCmdRead},

      cmdWS{true}
   {
   }

   //
   // Shell destructor
   //
   Shell::~Shell()
   {
   }

   //
   // Shell::cmdEndWS
   //
   void Shell::cmdEndWS()
   {
      if(cmdWS)
      {
         cmdIdx.push_back(cmdBuf.size());
         cmdWS = false;
      }
   }

   //
   // Shell::cmdPutWS
   //
   void Shell::cmdPutWS()
   {
      if(!cmdWS)
      {
         cmdBuf.push_back('\0');
         cmdWS = true;
      }
   }

   //
   // Shell::exec
   //
   void Shell::exec()
   {
      while((this->*state)()) {}
   }

   //
   // Shell::execCmdDone
   //
   bool Shell::execCmdDone()
   {
      cmdArg.clear();
      cmdBuf.clear();
      cmdIdx.clear();
      state = &Shell::execCmdRead;

      return true;
   }

   //
   // Shell::execCmdExec
   //
   bool Shell::execCmdExec()
   {
      if(auto cmd = Commands.findVal(&cmdBuf[cmdIdx[0]]))
      {
         cmdArg.reserve(cmdIdx.size());
         for(auto idx : cmdIdx)
            cmdArg.push_back(&cmdBuf[idx]);

         cmd(this, cmdArg.data(), cmdArg.size());
      }
      else
      {
         std::cerr << "Command not found: '" << &cmdBuf[cmdIdx[0]] << "'\n";
      }

      state = &Shell::execCmdDone;

      return true;
   }

   //
   // Shell::execCmdRead
   //
   bool Shell::execCmdRead()
   {
      for(char c; StdIn_Read(&c, 1);) switch(c)
      {
      case ' ':
      case '\t':
         cmdPutWS();
         break;

      case '\n':
         // Ignore lines with only whitespace.
         if(cmdIdx.empty())
            break;

         cmdPutWS();
         state = &Shell::execCmdExec;
         return true;

      case '\\':
         state = &Shell::execCmdReadES;
         return true;

      case '\"':
         cmdEndWS();
         // TODO: Double-quoted strings.
         break;

      case '\'':
         cmdEndWS();
         state = &Shell::execCmdReadQS;
         return true;

      default:
         cmdEndWS();
         cmdBuf.push_back(c);
         break;
      }

      return false;
   }

   //
   // Shell::execCmdReadES
   //
   bool Shell::execCmdReadES()
   {
      for(char c; StdIn_Read(&c, 1);) switch(c)
      {
      default:
         cmdEndWS();
         cmdBuf.push_back(c);
      case '\n':
         state = &Shell::execCmdRead;
         return true;
      }

      return false;
   }

   //
   // Shell::execCmdReadQS
   //
   bool Shell::execCmdReadQS()
   {
      for(char c; StdIn_Read(&c, 1);) switch(c)
      {
      case '\'':
         state = &Shell::execCmdRead;
         return true;

      default:
         cmdBuf.push_back(c);
         break;
      }

      return false;
   }
}

// EOF

