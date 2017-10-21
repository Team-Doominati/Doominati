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

#ifndef DGE__Code__MemStr_H__
#define DGE__Code__MemStr_H__

#include "Code/MemPtr.hpp"

#include <memory>


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Code
{
   void MemStrNCpy(MemPtr<Byte> out, Word outL, char const *in, std::size_t inL);

   std::unique_ptr<char[]> MemStrDup(MemPtr<Byte const> str);
   std::unique_ptr<char[]> MemStrDup(MemPtr<Byte const> str, std::size_t len);

   std::size_t MemStrLen(MemPtr<Byte const> str);
}

#endif//DGE__Code__MemStr_H__

