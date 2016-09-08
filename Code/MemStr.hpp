//-----------------------------------------------------------------------------
//
// Copyright (C) 2016 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Script VM memory strings utilities.
//
//----------------------------------------------------------------------------- 

#ifndef Doom__Code__MemStr_H__
#define Doom__Code__MemStr_H__

#include "Code/MemPtr.hpp"

#include <memory>


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace Doom
{
   namespace Code
   {
      std::unique_ptr<char[]> MemStrDup(MemPtr<Byte const> str);
      std::unique_ptr<char[]> MemStrDup(MemPtr<Byte const> str, std::size_t len);

      std::size_t MemStrLen(MemPtr<Byte const> str);
   }
}

#endif//Doom__Code__MemStr_H__

