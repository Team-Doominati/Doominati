//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Non-blocking stdin.
//
//-----------------------------------------------------------------------------

#ifndef DGE__Sh__StdIn_H__
#define DGE__Sh__StdIn_H__

#include "../Sh/Types.hpp"


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Sh
{
   void StdIn_Init();

   std::size_t StdIn_Read(char *buf, std::size_t len);

   int StdIn_UnGet(int c);
}

#endif//DGE__Sh__StdIn_H__

