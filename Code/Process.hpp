//-----------------------------------------------------------------------------
//
// Copyright (C) 2016 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Script VM processes.
//
//-----------------------------------------------------------------------------

#ifndef DGE__Code__Process_H__
#define DGE__Code__Process_H__

#include "Code/Types.hpp"

#include "Core/List.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::Code
{
   //
   // Process
   //
   class Process
   {
   public:
      Process(Program *prog);
      ~Process();

      void exec();

      Core::ListLink<Thread> threads;

      Program *const prog;
   };
}

#endif//DGE__Code__Process_H__

