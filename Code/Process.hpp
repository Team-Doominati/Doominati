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

#ifndef Doom__Code__Process_H__
#define Doom__Code__Process_H__

#include "Code/Types.hpp"

#include "Core/List.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace Doom
{
   namespace Code
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
}

#endif//Doom__Code__Process_H__

