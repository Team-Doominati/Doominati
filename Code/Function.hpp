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

#ifndef Doom__Code__Functions_H__
#define Doom__Code__Functions_H__

#include "Code/Types.hpp"

#include <GDCC/Core/String.hpp>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace Doom
{
   namespace Code
   {
      //
      // Function
      //
      class Function
      {
      public:
         OpCode *entry;
         Word    local;
         Word    param;
      };
   }
}

#endif//Doom__Code__Functions_H__

