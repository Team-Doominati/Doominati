//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Memory allocation
//
//-----------------------------------------------------------------------------

#ifndef DGE__Core__Alloc_H__
#define DGE__Core__Alloc_H__

#include "../Core/Types.hpp"


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Core
{
   //
   // AlignOffset
   //
   template<typename T>
   constexpr std::size_t AlignOffset(std::size_t off)
   {
      return off % alignof(T) ? off + (alignof(T) - off % alignof(T)) : off;
   }
}

#endif//DGE__Core__Alloc_H__

