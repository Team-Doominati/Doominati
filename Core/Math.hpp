//-----------------------------------------------------------------------------
//
// Copyright (C) 2016 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Inline math.
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


//----------------------------------------------------------------------------|
// Static Objects                                                             |
//

namespace Doom
{
   namespace Core
   {
      constexpr float pi = 3.14159265359f;
      constexpr float pi2 = pi / 2.0f;
      constexpr float pi4 = pi / 4.0f;
   }
}

#endif//Doom__Core__Math_H__

