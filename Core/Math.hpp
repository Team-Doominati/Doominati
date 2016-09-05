//-----------------------------------------------------------------------------
//
// Copyright (C) 2016 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Inline math functions.
//
//-----------------------------------------------------------------------------

#ifndef Doom__Core__Math_H__
#define Doom__Core__Math_H__


//----------------------------------------------------------------------------|
// Static Functions                                                           |
//

namespace Doom
{
   namespace Core
   {
      constexpr float Lerp(float x, float y, float amt)
      {
         return ((1.0f - amt) * x) + (amt * y);
      }
   }
}

#endif//Doom__Core__Math_H__

