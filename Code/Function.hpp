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

#ifndef DGE__Code__Functions_H__
#define DGE__Code__Functions_H__

#include "Code/Types.hpp"

#include <GDCC/Core/String.hpp>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::Code
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

#endif//DGE__Code__Functions_H__

