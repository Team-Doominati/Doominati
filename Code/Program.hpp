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

#ifndef Doom__Code__Program_H__
#define Doom__Code__Program_H__

#include "Code/Function.hpp"
#include "Code/OpCode.hpp"
#include "Code/Types.hpp"

#include <GDCC/Core/Array.hpp>
#include <GDCC/Core/String.hpp>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace Doom
{
   namespace Code
   {
      //
      // Program
      //
      class Program
      {
      public:
         GDCC::Core::Array<OpCode>   codes;
         GDCC::Core::Array<Function> funcByIdx;
      };
   }
}

#endif//Doom__Code__Program_H__

