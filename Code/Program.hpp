//-----------------------------------------------------------------------------
//
// Copyright (C) 2016 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Script VM functions.
//
//-----------------------------------------------------------------------------

#ifndef DGE__Code__Program_H__
#define DGE__Code__Program_H__

#include "Code/Function.hpp"
#include "Code/Memory.hpp"
#include "Code/OpCode.hpp"
#include "Code/Types.hpp"

#include "Core/HashMapFixed.hpp"

#include <GDCC/Core/Array.hpp>
#include <GDCC/Core/String.hpp>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::Code
{
   //
   // Program
   //
   class Program
   {
   public:
      GDCC::Core::Array<OpCode>                         codes;
      Core::HashMapFixed<GDCC::Core::String, Function>  funcs;
      GDCC::Core::Array<Core::HashMapFixed<Word, Word>> jumps;

      // This belongs in Process some day.
      Memory memory;

      Word freestoreBegin;
      Word freestoreEnd;
   };
}

#endif//DGE__Code__Program_H__

