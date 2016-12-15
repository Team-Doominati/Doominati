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

#ifndef DGE__GL__Color_H__
#define DGE__GL__Color_H__

#include "Core/Math.hpp"

#include <utility>
#include <unordered_map>
#include <string>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::GL
{
   //
   // Color
   //
   class Color
   {
   public:
      Color() = default;
      Color(float r_, float g_, float b_, float a_ = 1.0f) :
         r{r_}, g{g_}, b{b_}, a{a_} {}
      Color(Color const &) = default;
      Color(Color &&) = default;

      Color &operator=(Color const &) = default;
      Color &operator=(Color &&) = default;

      Color &fromHSV(float h, float s, float v, float a_ = 1.0f)
         {return *this = Color::FromHSV(h, s, v, a_);}

      Color &interpolate(Color const &c, float amt);
      float &operator[](std::size_t x);

      float r = 1.0f;
      float g = 1.0f;
      float b = 1.0f;
      float a = 1.0f;


      static Color FromHSV(float h, float s, float v, float a = 1.0f);
      static Color Interpolate(Color const &a, Color const &b, float amt);
   };
}


//----------------------------------------------------------------------------|
// Extern Objects                                                             |
//

namespace DGE::GL
{
   extern std::unordered_map<std::string, Color> const Colors;
}

#endif//DGE__GL__Color_H__

