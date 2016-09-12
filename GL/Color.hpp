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
#include "Core/Math.hpp"


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
         Color() = default;
         Color(float r_, float g_, float b_, float a_ = 1.0f) : r{r_}, g{g_}, b{b_}, a{a_} {}

         static Color FromHSV(float h, float s, float v, float a = 1.0f);
         static Color Interpolate(Color const &a, Color const &b, float amt);

         Color &fromHSV(float h, float s, float v, float aa = 1.0f)
         {
            return *this = Color::FromHSV(h, s, v, aa);
         }

         Color &interpolate(Color const &other, float amt)
         {
            r = Core::Lerp(r, other.r, amt);
            g = Core::Lerp(g, other.g, amt);
            b = Core::Lerp(b, other.b, amt);
            a = Core::Lerp(a, other.a, amt);

            return *this;
         }

         float r = 1.0f;
         float g = 1.0f;
         float b = 1.0f;
         float a = 1.0f;

         static Color const AliceBlue;
         static Color const AntiqueWhite;
         static Color const Aqua;
         static Color const Aquamarine;
         static Color const Azure;
         static Color const Beige;
         static Color const Bisque;
         static Color const Black;
         static Color const BlanchedAlmond;
         static Color const Blue;
         static Color const BlueViolet;
         static Color const Brown;
         static Color const Burlywood;
         static Color const CadetBlue;
         static Color const Chartreuse;
         static Color const Chocolate;
         static Color const Coral;
         static Color const Cornflower;
         static Color const Cornsilk;
         static Color const Crimson;
         static Color const Cyan;
         static Color const DarkBlue;
         static Color const DarkCyan;
         static Color const DarkGoldenrod;
         static Color const DarkGray;
         static Color const DarkGreen;
         static Color const DarkKhaki;
         static Color const DarkMagenta;
         static Color const DarkOliveGreen;
         static Color const DarkOrange;
         static Color const DarkOrchid;
         static Color const DarkRed;
         static Color const DarkSalmon;
         static Color const DarkSeaGreen;
         static Color const DarkSlateBlue;
         static Color const DarkSlateGray;
         static Color const DarkTurquoise;
         static Color const DarkViolet;
         static Color const DeepPink;
         static Color const DeepSkyBlue;
         static Color const DimGray;
         static Color const DodgerBlue;
         static Color const Firebrick;
         static Color const FloralWhite;
         static Color const ForestGreen;
         static Color const Fuchsia;
         static Color const Gainsboro;
         static Color const GhostWhite;
         static Color const Gold;
         static Color const Goldenrod;
         static Color const Gray;
         static Color const WebGray;
         static Color const Green;
         static Color const WebGreen;
         static Color const GreenYellow;
         static Color const Honeydew;
         static Color const HotPink;
         static Color const IndianRed;
         static Color const Indigo;
         static Color const Ivory;
         static Color const Khaki;
         static Color const Lavender;
         static Color const LavenderBlush;
         static Color const LawnGreen;
         static Color const LemonChiffon;
         static Color const LightBlue;
         static Color const LightCoral;
         static Color const LightCyan;
         static Color const LightGoldenrod;
         static Color const LightGray;
         static Color const LightGreen;
         static Color const LightPink;
         static Color const LightSalmon;
         static Color const LightSeaGreen;
         static Color const LightSkyBlue;
         static Color const LightSlateGray;
         static Color const LightSteelBlue;
         static Color const LightYellow;
         static Color const Lime;
         static Color const LimeGreen;
         static Color const Linen;
         static Color const Magenta;
         static Color const Maroon;
         static Color const WebMaroon;
         static Color const MediumAquamarine;
         static Color const MediumBlue;
         static Color const MediumOrchid;
         static Color const MediumPurple;
         static Color const MediumSeaGreen;
         static Color const MediumSlateBlue;
         static Color const MediumSpringGreen;
         static Color const MediumTurquoise;
         static Color const MediumVioletRed;
         static Color const MidnightBlue;
         static Color const MintCream;
         static Color const MistyRose;
         static Color const Moccasin;
         static Color const NavajoWhite;
         static Color const NavyBlue;
         static Color const OldLace;
         static Color const Olive;
         static Color const OliveDrab;
         static Color const Orange;
         static Color const OrangeRed;
         static Color const Orchid;
         static Color const PaleGoldenrod;
         static Color const PaleGreen;
         static Color const PaleTurquoise;
         static Color const PaleVioletRed;
         static Color const PapayaWhip;
         static Color const PeachPuff;
         static Color const Peru;
         static Color const Pink;
         static Color const Plum;
         static Color const PowderBlue;
         static Color const Purple;
         static Color const WebPurple;
         static Color const RebeccaPurple;
         static Color const Red;
         static Color const RosyBrown;
         static Color const RoyalBlue;
         static Color const SaddleBrown;
         static Color const Salmon;
         static Color const SandyBrown;
         static Color const SeaGreen;
         static Color const Seashell;
         static Color const Sienna;
         static Color const Silver;
         static Color const SkyBlue;
         static Color const SlateBlue;
         static Color const SlateGray;
         static Color const Snow;
         static Color const SpringGreen;
         static Color const SteelBlue;
         static Color const Tan;
         static Color const Teal;
         static Color const Thistle;
         static Color const Tomato;
         static Color const Turquoise;
         static Color const Violet;
         static Color const Wheat;
         static Color const White;
         static Color const WhiteSmoke;
         static Color const Yellow;
         static Color const YellowGreen;
         static Color const Zero;
         static Color const ZeroWhite;
      };
   }
}

#endif//Doom__GL__Color_H__

