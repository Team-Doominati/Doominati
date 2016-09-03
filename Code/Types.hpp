//-----------------------------------------------------------------------------
//
// Copyright (C) 2016 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Common typedefs.
//
//-----------------------------------------------------------------------------

#ifndef Doom__Code__Types_H__
#define Doom__Code__Types_H__

#include <cstdint>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace Doom
{
   namespace Code
   {
      class Function;
      class OpCode;
      class Process;
      class Program;
      class Task;
      class Thread;

      using Byte  = std::uint8_t;
      using HWord = std::uint16_t;
      using Word  = std::uint32_t;

      using Native = bool (*)(Task *, Word const *, Word);
   }
}

#endif//Doom__Code__Types_H__

