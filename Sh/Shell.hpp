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

#ifndef DGE__Sh__Shell_H__
#define DGE__Sh__Shell_H__

#include "../Sh/Types.hpp"

#include <vector>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::Sh
{
   //
   // Shell
   //
   class Shell
   {
   public:
      Shell();
      ~Shell();

      void exec();

   private:
      void cmdEndWS();
      void cmdPutWS();

      bool execCmdDone();
      bool execCmdExec();
      bool execCmdRead();
      bool execCmdReadES();
      bool execCmdReadQS();

      std::vector<char *>      cmdArg;
      std::vector<char>        cmdBuf;
      std::vector<std::size_t> cmdIdx;

      bool (Shell::*state)();

      bool cmdWS;
   };
}

#endif//DGE__Sh__Shell_H__

