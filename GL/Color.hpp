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
         Color(double r_, double g_, double b_, double a_ = 1.0) : r{r_}, g{g_}, b{b_}, a{a_} {}
         
         static Color FromHSV(double h, double s, double v, double a = 1.0);
         static Color Interpolate(Color const &a, Color const &b, double amt);
         
         Color &fromHSV(double h, double s, double v, double aa = 1.0)
         {
            return *this = std::move(Color::FromHSV(h, s, v, aa));
         }
         
         Color &interpolate(Color const &other, double amt)
         {
            r = ((1.0 - amt) * r) + (amt * other.r);
            g = ((1.0 - amt) * g) + (amt * other.g);
            b = ((1.0 - amt) * b) + (amt * other.b);
            a = ((1.0 - amt) * a) + (amt * other.a);
            
            return *this;
         }
         
         double r = 1.0;
         double g = 1.0;
         double b = 1.0;
         double a = 1.0;
      };
   }
}

#endif//Doom__GL__Color_H__

