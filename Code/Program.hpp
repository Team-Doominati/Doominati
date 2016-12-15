//-----------------------------------------------------------------------------
//
// Copyright (C) 2016 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Script VM programs.
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
#include <GDCC/Core/Origin.hpp>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::Code
{
   //
   // Origin
   //
   class Origin
   {
   public:
      GDCC::Core::Origin file;
      GDCC::Core::String func;
   };

   //
   // Program
   //
   class Program
   {
   public:
      GDCC::Core::String findOriginFunc(std::size_t code);

      Origin getOrigin(OpCode const *code) {return getOrigin(code - codes.data());}
      Origin getOrigin(std::size_t code);

      GDCC::Core::Array<OpCode>                         codes;
      Core::HashMapFixed<GDCC::Core::String, Function>  funcs;
      GDCC::Core::Array<Core::HashMapFixed<Word, Word>> jumps;
      GDCC::Core::Array<Origin>                         origs;

      // This belongs in Process some day.
      Memory memory;

      Word freestoreBegin;
      Word freestoreEnd;
   };
}

#endif//DGE__Code__Program_H__

