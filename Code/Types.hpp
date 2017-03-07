//-----------------------------------------------------------------------------
//
// Copyright (C) 2016-2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Common typedefs.
//
//-----------------------------------------------------------------------------

#ifndef DGE__Code__Types_H__
#define DGE__Code__Types_H__

#include <cstdint>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::Code
{
   class Callback;
   class Function;
   class Loader;
   class OpCode;
   class Process;
   class Program;
   class Task;
   class Thread;

   using Byte  = std::uint8_t;
   using DWord = std::uint64_t;
   using HWord = std::uint16_t;
   using SWord = std::int32_t;
   using Word  = std::uint32_t;

   using Native = bool (*)(Task *, Word const *, Word);
}

#endif//DGE__Code__Types_H__

