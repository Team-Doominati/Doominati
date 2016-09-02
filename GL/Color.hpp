//-----------------------------------------------------------------------------
//
// Copyright (C) 2016 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Color management.
//
//-----------------------------------------------------------------------------

#ifndef Doom__GL__Color_H__
#define Doom__GL__Color_H__

#include <utility>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace Doom
{
   namespace GL
   {
      //
      // Color
      //

      class Color
      {
      public:
         Color(float r_, float g_, float b_, float a_ = 1.0f) : r{r_}, g{g_}, b{b_}, a{a_} {}

         static Color FromHSV(float h, float s, float v, float a = 1.0f);
         static Color Interpolate(Color const &a, Color const &b, float amt);

         Color &fromHSV(float h, float s, float v, float aa = 1.0f)
         {
            return *this = Color::FromHSV(h, s, v, aa);
         }

         Color &interpolate(Color const &other, float amt)
         {
            r = ((1.0f - amt) * r) + (amt * other.r);
            g = ((1.0f - amt) * g) + (amt * other.g);
            b = ((1.0f - amt) * b) + (amt * other.b);
            a = ((1.0f - amt) * a) + (amt * other.a);

            return *this;
         }

         float r = 1.0f;
         float g = 1.0f;
         float b = 1.0f;
         float a = 1.0f;
      };
   }
}

#endif//Doom__GL__Color_H__

