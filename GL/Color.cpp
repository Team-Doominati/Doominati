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

#include "GL/Color.hpp"

#include <cmath>


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace Doom
{
   namespace GL
   {
      //
      // Color::FromHSV
      //
      
      Color Color::FromHSV(double h, double s, double v, double a)
      {
         h *= 360.0;
         
         double chroma = v * s;
         double hp = h / 60.0;
         double x = chroma * (1 - std::abs(std::fmod(hp, 2) - 1));
         double r1 = 0, g1 = 0, b1 = 0;
         
              if(0 <= hp && hp < 1) r1 = chroma, g1 = x,      b1 = 0;
         else if(1 <= hp && hp < 2) r1 = x,      g1 = chroma, b1 = 0;
         else if(2 <= hp && hp < 3) r1 = 0,      g1 = chroma, b1 = x;
         else if(3 <= hp && hp < 4) r1 = 0,      g1 = x,      b1 = chroma;
         else if(4 <= hp && hp < 5) r1 = x,      g1 = 0,      b1 = chroma;
         else if(5 <= hp && hp < 6) r1 = chroma, g1 = 0,      b1 = x;
         
         double m = v - chroma;
         return { r1 + m, g1 + m, b1 + m, a };
      }
      
      //
      // Color::Interpolate
      //
      
      Color Color::Interpolate(Color const &a, Color const &b, double amt)
      {
         return {
            ((1.0 - amt) * a.r) + (amt * b.r),
            ((1.0 - amt) * a.g) + (amt * b.g),
            ((1.0 - amt) * a.b) + (amt * b.b),
            ((1.0 - amt) * a.a) + (amt * b.a)
         };
      }
   }
}


// EOF
