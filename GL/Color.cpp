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
// Extern Objects                                                             |
//

namespace DGE::GL
{
   std::unordered_map<std::string, Color> const Colors = {
      {"AliceBlue", {0.941, 0.973, 1.000}},
      {"AntiqueWhite", {0.980, 0.922, 0.843}},
      {"Aqua", {0.000, 1.000, 1.000}},
      {"Aquamarine", {0.498, 1.000, 0.831}},
      {"Azure", {0.941, 1.000, 1.000}},
      {"Beige", {0.961, 0.961, 0.863}},
      {"Bisque", {1.000, 0.894, 0.769}},
      {"Black", {0.000, 0.000, 0.000}},
      {"BlanchedAlmond", {1.000, 0.922, 0.804}},
      {"Blue", {0.000, 0.000, 1.000}},
      {"BlueViolet", {0.541, 0.169, 0.886}},
      {"Brown", {0.647, 0.165, 0.165}},
      {"Burlywood", {0.871, 0.722, 0.529}},
      {"CadetBlue", {0.373, 0.620, 0.627}},
      {"Chartreuse", {0.498, 1.000, 0.000}},
      {"Chocolate", {0.824, 0.412, 0.118}},
      {"Coral", {1.000, 0.498, 0.314}},
      {"Cornflower", {0.392, 0.584, 0.929}},
      {"Cornsilk", {1.000, 0.973, 0.863}},
      {"Crimson", {0.863, 0.078, 0.235}},
      {"Cyan", {0.000, 1.000, 1.000}},
      {"DarkBlue", {0.000, 0.000, 0.545}},
      {"DarkCyan", {0.000, 0.545, 0.545}},
      {"DarkGoldenrod", {0.722, 0.525, 0.043}},
      {"DarkGray", {0.663, 0.663, 0.663}},
      {"DarkGreen", {0.000, 0.392, 0.000}},
      {"DarkKhaki", {0.741, 0.718, 0.420}},
      {"DarkMagenta", {0.545, 0.000, 0.545}},
      {"DarkOliveGreen", {0.333, 0.420, 0.184}},
      {"DarkOrange", {1.000, 0.549, 0.000}},
      {"DarkOrchid", {0.600, 0.196, 0.800}},
      {"DarkRed", {0.545, 0.000, 0.000}},
      {"DarkSalmon", {0.914, 0.588, 0.478}},
      {"DarkSeaGreen", {0.561, 0.737, 0.561}},
      {"DarkSlateBlue", {0.282, 0.239, 0.545}},
      {"DarkSlateGray", {0.184, 0.310, 0.310}},
      {"DarkTurquoise", {0.000, 0.808, 0.820}},
      {"DarkViolet", {0.580, 0.000, 0.827}},
      {"DeepPink", {1.000, 0.078, 0.576}},
      {"DeepSkyBlue", {0.000, 0.749, 1.000}},
      {"DimGray", {0.412, 0.412, 0.412}},
      {"DodgerBlue", {0.118, 0.565, 1.000}},
      {"Firebrick", {0.698, 0.133, 0.133}},
      {"FloralWhite", {1.000, 0.980, 0.941}},
      {"ForestGreen", {0.133, 0.545, 0.133}},
      {"Fuchsia", {1.000, 0.000, 1.000}},
      {"Gainsboro", {0.863, 0.863, 0.863}},
      {"GhostWhite", {0.973, 0.973, 1.000}},
      {"Gold", {1.000, 0.843, 0.000}},
      {"Goldenrod", {0.855, 0.647, 0.125}},
      {"Gray", {0.745, 0.745, 0.745}},
      {"WebGray", {0.502, 0.502, 0.502}},
      {"Green", {0.000, 1.000, 0.000}},
      {"WebGreen", {0.000, 0.502, 0.000}},
      {"GreenYellow", {0.678, 1.000, 0.184}},
      {"Honeydew", {0.941, 1.000, 0.941}},
      {"HotPink", {1.000, 0.412, 0.706}},
      {"IndianRed", {0.804, 0.361, 0.361}},
      {"Indigo", {0.294, 0.000, 0.510}},
      {"Ivory", {1.000, 1.000, 0.941}},
      {"Khaki", {0.941, 0.902, 0.549}},
      {"Lavender", {0.902, 0.902, 0.980}},
      {"LavenderBlush", {1.000, 0.941, 0.961}},
      {"LawnGreen", {0.486, 0.988, 0.000}},
      {"LemonChiffon", {1.000, 0.980, 0.804}},
      {"LightBlue", {0.678, 0.847, 0.902}},
      {"LightCoral", {0.941, 0.502, 0.502}},
      {"LightCyan", {0.878, 1.000, 1.000}},
      {"LightGoldenrod", {0.980, 0.980, 0.824}},
      {"LightGray", {0.827, 0.827, 0.827}},
      {"LightGreen", {0.565, 0.933, 0.565}},
      {"LightPink", {1.000, 0.714, 0.757}},
      {"LightSalmon", {1.000, 0.627, 0.478}},
      {"LightSeaGreen", {0.125, 0.698, 0.667}},
      {"LightSkyBlue", {0.529, 0.808, 0.980}},
      {"LightSlateGray", {0.467, 0.533, 0.600}},
      {"LightSteelBlue", {0.690, 0.769, 0.871}},
      {"LightYellow", {1.000, 1.000, 0.878}},
      {"Lime", {0.000, 1.000, 0.000}},
      {"LimeGreen", {0.196, 0.804, 0.196}},
      {"Linen", {0.980, 0.941, 0.902}},
      {"Magenta", {1.000, 0.000, 1.000}},
      {"Maroon", {0.690, 0.188, 0.376}},
      {"WebMaroon", {0.498, 0.000, 0.000}},
      {"MediumAquamarine", {0.400, 0.804, 0.667}},
      {"MediumBlue", {0.000, 0.000, 0.804}},
      {"MediumOrchid", {0.729, 0.333, 0.827}},
      {"MediumPurple", {0.576, 0.439, 0.859}},
      {"MediumSeaGreen", {0.235, 0.702, 0.443}},
      {"MediumSlateBlue", {0.482, 0.408, 0.933}},
      {"MediumSpringGreen", {0.000, 0.980, 0.604}},
      {"MediumTurquoise", {0.282, 0.820, 0.800}},
      {"MediumVioletRed", {0.780, 0.082, 0.522}},
      {"MidnightBlue", {0.098, 0.098, 0.439}},
      {"MintCream", {0.961, 1.000, 0.980}},
      {"MistyRose", {1.000, 0.894, 0.882}},
      {"Moccasin", {1.000, 0.894, 0.710}},
      {"NavajoWhite", {1.000, 0.871, 0.678}},
      {"NavyBlue", {0.000, 0.000, 0.502}},
      {"OldLace", {0.992, 0.961, 0.902}},
      {"Olive", {0.502, 0.502, 0.000}},
      {"OliveDrab", {0.420, 0.557, 0.137}},
      {"Orange", {1.000, 0.647, 0.000}},
      {"OrangeRed", {1.000, 0.271, 0.000}},
      {"Orchid", {0.855, 0.439, 0.839}},
      {"PaleGoldenrod", {0.933, 0.910, 0.667}},
      {"PaleGreen", {0.596, 0.984, 0.596}},
      {"PaleTurquoise", {0.686, 0.933, 0.933}},
      {"PaleVioletRed", {0.859, 0.439, 0.576}},
      {"PapayaWhip", {1.000, 0.937, 0.835}},
      {"PeachPuff", {1.000, 0.855, 0.725}},
      {"Peru", {0.804, 0.522, 0.247}},
      {"Pink", {1.000, 0.753, 0.796}},
      {"Plum", {0.867, 0.627, 0.867}},
      {"PowderBlue", {0.690, 0.878, 0.902}},
      {"Purple", {0.627, 0.125, 0.941}},
      {"WebPurple", {0.498, 0.000, 0.498}},
      {"RebeccaPurple", {0.400, 0.200, 0.600}},
      {"Red", {1.000, 0.000, 0.000}},
      {"RosyBrown", {0.737, 0.561, 0.561}},
      {"RoyalBlue", {0.255, 0.412, 0.882}},
      {"SaddleBrown", {0.545, 0.271, 0.075}},
      {"Salmon", {0.980, 0.502, 0.447}},
      {"SandyBrown", {0.957, 0.643, 0.376}},
      {"SeaGreen", {0.180, 0.545, 0.341}},
      {"Seashell", {1.000, 0.961, 0.933}},
      {"Sienna", {0.627, 0.322, 0.176}},
      {"Silver", {0.753, 0.753, 0.753}},
      {"SkyBlue", {0.529, 0.808, 0.922}},
      {"SlateBlue", {0.416, 0.353, 0.804}},
      {"SlateGray", {0.439, 0.502, 0.565}},
      {"Snow", {1.000, 0.980, 0.980}},
      {"SpringGreen", {0.000, 1.000, 0.498}},
      {"SteelBlue", {0.275, 0.510, 0.706}},
      {"Tan", {0.824, 0.706, 0.549}},
      {"Teal", {0.000, 0.502, 0.502}},
      {"Thistle", {0.847, 0.749, 0.847}},
      {"Tomato", {1.000, 0.388, 0.278}},
      {"Turquoise", {0.251, 0.878, 0.816}},
      {"Violet", {0.933, 0.510, 0.933}},
      {"Wheat", {0.961, 0.871, 0.702}},
      {"White", {1.000, 1.000, 1.000}},
      {"WhiteSmoke", {0.961, 0.961, 0.961}},
      {"Yellow", {1.000, 1.000, 0.000}},
      {"YellowGreen", {0.604, 0.804, 0.196}},
      {"Zero", {0.0, 0.0, 0.0, 0.0}},
      {"ZeroWhite", {1.0, 1.0, 1.0, 0.0}}
   };
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::GL
{
   //
   // Color::interpolate
   //
   Color &Color::interpolate(Color const &c, float amt)
   {
      r = Core::Lerp(r, c.r, amt);
      g = Core::Lerp(g, c.g, amt);
      b = Core::Lerp(b, c.b, amt);
      a = Core::Lerp(a, c.a, amt);

      return *this;
   }

   //
   // Color::operator[]
   //
   float &Color::operator[](std::size_t x)
   {
      switch(x)
      {
      default: case 0: return r;
               case 1: return g;
               case 2: return b;
               case 3: return a;
      }
   }

   //
   // Color::FromHSV
   //
   Color Color::FromHSV(float h, float s, float v, float a)
   {
      h *= 360.0f;

           if(h < 0)     h = 0;
      else if(h > 359.5) h = 359.5; // thanks floating point-bama

      float chroma = v * s;
      float hp = h / 60.0f;
      float x = chroma * (1 - std::abs(std::fmod(hp, 2) - 1));
      float r1 = 0, g1 = 0, b1 = 0;

           if(0 <= hp && hp < 1) r1 = chroma, g1 = x,      b1 = 0;
      else if(1 <= hp && hp < 2) r1 = x,      g1 = chroma, b1 = 0;
      else if(2 <= hp && hp < 3) r1 = 0,      g1 = chroma, b1 = x;
      else if(3 <= hp && hp < 4) r1 = 0,      g1 = x,      b1 = chroma;
      else if(4 <= hp && hp < 5) r1 = x,      g1 = 0,      b1 = chroma;
      else if(5 <= hp && hp < 6) r1 = chroma, g1 = 0,      b1 = x;

      float m = v - chroma;
      return { r1 + m, g1 + m, b1 + m, a };
   }

   //
   // Color::Interpolate
   //
   Color Color::Interpolate(Color const &a, Color const &b, float amt)
   {
      return {
         Core::Lerp(a.r, b.r, amt),
         Core::Lerp(a.g, b.g, amt),
         Core::Lerp(a.b, b.b, amt),
         Core::Lerp(a.a, b.a, amt)
      };
   }
}

// EOF
