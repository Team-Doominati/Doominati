//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Script struct to/from host struct conversion.
//
//-----------------------------------------------------------------------------

#ifndef DGE__Code__ConvertStruct_H__
#define DGE__Code__ConvertStruct_H__

#include "Code/Convert.hpp"

#include "Core/Color.hpp"
#include "Core/Vector2.hpp"


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Code
{
   //
   // ColorToHost
   //
   constexpr Core::Color ColorToHost(Word r, Word g, Word b, Word a)
   {
      return {static_cast<float>(ULFractToHost(r)),
              static_cast<float>(ULFractToHost(g)),
              static_cast<float>(ULFractToHost(b)),
              static_cast<float>(ULFractToHost(a))};
   }

   //
   // Point2ToHost
   //
   constexpr Core::Vector2 Point2ToHost(Word x, Word y)
   {
      return {static_cast<float>(SAccumToHost(x)),
              static_cast<float>(SAccumToHost(y))};
   }
}

#endif//DGE__Code__ConvertStruct_H__

