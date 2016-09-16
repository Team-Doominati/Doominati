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
            ((1.0f - amt) * a.r) + (amt * b.r),
            ((1.0f - amt) * a.g) + (amt * b.g),
            ((1.0f - amt) * a.b) + (amt * b.b),
            ((1.0f - amt) * a.a) + (amt * b.a)
         };
      }

      //
      // Color constants
      //

      Color const Color::AliceBlue{0.941, 0.973, 1.000};
      Color const Color::AntiqueWhite{0.980, 0.922, 0.843};
      Color const Color::Aqua{0.000, 1.000, 1.000};
      Color const Color::Aquamarine{0.498, 1.000, 0.831};
      Color const Color::Azure{0.941, 1.000, 1.000};
      Color const Color::Beige{0.961, 0.961, 0.863};
      Color const Color::Bisque{1.000, 0.894, 0.769};
      Color const Color::Black{0.000, 0.000, 0.000};
      Color const Color::BlanchedAlmond{1.000, 0.922, 0.804};
      Color const Color::Blue{0.000, 0.000, 1.000};
      Color const Color::BlueViolet{0.541, 0.169, 0.886};
      Color const Color::Brown{0.647, 0.165, 0.165};
      Color const Color::Burlywood{0.871, 0.722, 0.529};
      Color const Color::CadetBlue{0.373, 0.620, 0.627};
      Color const Color::Chartreuse{0.498, 1.000, 0.000};
      Color const Color::Chocolate{0.824, 0.412, 0.118};
      Color const Color::Coral{1.000, 0.498, 0.314};
      Color const Color::Cornflower{0.392, 0.584, 0.929};
      Color const Color::Cornsilk{1.000, 0.973, 0.863};
      Color const Color::Crimson{0.863, 0.078, 0.235};
      Color const Color::Cyan{0.000, 1.000, 1.000};
      Color const Color::DarkBlue{0.000, 0.000, 0.545};
      Color const Color::DarkCyan{0.000, 0.545, 0.545};
      Color const Color::DarkGoldenrod{0.722, 0.525, 0.043};
      Color const Color::DarkGray{0.663, 0.663, 0.663};
      Color const Color::DarkGreen{0.000, 0.392, 0.000};
      Color const Color::DarkKhaki{0.741, 0.718, 0.420};
      Color const Color::DarkMagenta{0.545, 0.000, 0.545};
      Color const Color::DarkOliveGreen{0.333, 0.420, 0.184};
      Color const Color::DarkOrange{1.000, 0.549, 0.000};
      Color const Color::DarkOrchid{0.600, 0.196, 0.800};
      Color const Color::DarkRed{0.545, 0.000, 0.000};
      Color const Color::DarkSalmon{0.914, 0.588, 0.478};
      Color const Color::DarkSeaGreen{0.561, 0.737, 0.561};
      Color const Color::DarkSlateBlue{0.282, 0.239, 0.545};
      Color const Color::DarkSlateGray{0.184, 0.310, 0.310};
      Color const Color::DarkTurquoise{0.000, 0.808, 0.820};
      Color const Color::DarkViolet{0.580, 0.000, 0.827};
      Color const Color::DeepPink{1.000, 0.078, 0.576};
      Color const Color::DeepSkyBlue{0.000, 0.749, 1.000};
      Color const Color::DimGray{0.412, 0.412, 0.412};
      Color const Color::DodgerBlue{0.118, 0.565, 1.000};
      Color const Color::Firebrick{0.698, 0.133, 0.133};
      Color const Color::FloralWhite{1.000, 0.980, 0.941};
      Color const Color::ForestGreen{0.133, 0.545, 0.133};
      Color const Color::Fuchsia{1.000, 0.000, 1.000};
      Color const Color::Gainsboro{0.863, 0.863, 0.863};
      Color const Color::GhostWhite{0.973, 0.973, 1.000};
      Color const Color::Gold{1.000, 0.843, 0.000};
      Color const Color::Goldenrod{0.855, 0.647, 0.125};
      Color const Color::Gray{0.745, 0.745, 0.745};
      Color const Color::WebGray{0.502, 0.502, 0.502};
      Color const Color::Green{0.000, 1.000, 0.000};
      Color const Color::WebGreen{0.000, 0.502, 0.000};
      Color const Color::GreenYellow{0.678, 1.000, 0.184};
      Color const Color::Honeydew{0.941, 1.000, 0.941};
      Color const Color::HotPink{1.000, 0.412, 0.706};
      Color const Color::IndianRed{0.804, 0.361, 0.361};
      Color const Color::Indigo{0.294, 0.000, 0.510};
      Color const Color::Ivory{1.000, 1.000, 0.941};
      Color const Color::Khaki{0.941, 0.902, 0.549};
      Color const Color::Lavender{0.902, 0.902, 0.980};
      Color const Color::LavenderBlush{1.000, 0.941, 0.961};
      Color const Color::LawnGreen{0.486, 0.988, 0.000};
      Color const Color::LemonChiffon{1.000, 0.980, 0.804};
      Color const Color::LightBlue{0.678, 0.847, 0.902};
      Color const Color::LightCoral{0.941, 0.502, 0.502};
      Color const Color::LightCyan{0.878, 1.000, 1.000};
      Color const Color::LightGoldenrod{0.980, 0.980, 0.824};
      Color const Color::LightGray{0.827, 0.827, 0.827};
      Color const Color::LightGreen{0.565, 0.933, 0.565};
      Color const Color::LightPink{1.000, 0.714, 0.757};
      Color const Color::LightSalmon{1.000, 0.627, 0.478};
      Color const Color::LightSeaGreen{0.125, 0.698, 0.667};
      Color const Color::LightSkyBlue{0.529, 0.808, 0.980};
      Color const Color::LightSlateGray{0.467, 0.533, 0.600};
      Color const Color::LightSteelBlue{0.690, 0.769, 0.871};
      Color const Color::LightYellow{1.000, 1.000, 0.878};
      Color const Color::Lime{0.000, 1.000, 0.000};
      Color const Color::LimeGreen{0.196, 0.804, 0.196};
      Color const Color::Linen{0.980, 0.941, 0.902};
      Color const Color::Magenta{1.000, 0.000, 1.000};
      Color const Color::Maroon{0.690, 0.188, 0.376};
      Color const Color::WebMaroon{0.498, 0.000, 0.000};
      Color const Color::MediumAquamarine{0.400, 0.804, 0.667};
      Color const Color::MediumBlue{0.000, 0.000, 0.804};
      Color const Color::MediumOrchid{0.729, 0.333, 0.827};
      Color const Color::MediumPurple{0.576, 0.439, 0.859};
      Color const Color::MediumSeaGreen{0.235, 0.702, 0.443};
      Color const Color::MediumSlateBlue{0.482, 0.408, 0.933};
      Color const Color::MediumSpringGreen{0.000, 0.980, 0.604};
      Color const Color::MediumTurquoise{0.282, 0.820, 0.800};
      Color const Color::MediumVioletRed{0.780, 0.082, 0.522};
      Color const Color::MidnightBlue{0.098, 0.098, 0.439};
      Color const Color::MintCream{0.961, 1.000, 0.980};
      Color const Color::MistyRose{1.000, 0.894, 0.882};
      Color const Color::Moccasin{1.000, 0.894, 0.710};
      Color const Color::NavajoWhite{1.000, 0.871, 0.678};
      Color const Color::NavyBlue{0.000, 0.000, 0.502};
      Color const Color::OldLace{0.992, 0.961, 0.902};
      Color const Color::Olive{0.502, 0.502, 0.000};
      Color const Color::OliveDrab{0.420, 0.557, 0.137};
      Color const Color::Orange{1.000, 0.647, 0.000};
      Color const Color::OrangeRed{1.000, 0.271, 0.000};
      Color const Color::Orchid{0.855, 0.439, 0.839};
      Color const Color::PaleGoldenrod{0.933, 0.910, 0.667};
      Color const Color::PaleGreen{0.596, 0.984, 0.596};
      Color const Color::PaleTurquoise{0.686, 0.933, 0.933};
      Color const Color::PaleVioletRed{0.859, 0.439, 0.576};
      Color const Color::PapayaWhip{1.000, 0.937, 0.835};
      Color const Color::PeachPuff{1.000, 0.855, 0.725};
      Color const Color::Peru{0.804, 0.522, 0.247};
      Color const Color::Pink{1.000, 0.753, 0.796};
      Color const Color::Plum{0.867, 0.627, 0.867};
      Color const Color::PowderBlue{0.690, 0.878, 0.902};
      Color const Color::Purple{0.627, 0.125, 0.941};
      Color const Color::WebPurple{0.498, 0.000, 0.498};
      Color const Color::RebeccaPurple{0.400, 0.200, 0.600};
      Color const Color::Red{1.000, 0.000, 0.000};
      Color const Color::RosyBrown{0.737, 0.561, 0.561};
      Color const Color::RoyalBlue{0.255, 0.412, 0.882};
      Color const Color::SaddleBrown{0.545, 0.271, 0.075};
      Color const Color::Salmon{0.980, 0.502, 0.447};
      Color const Color::SandyBrown{0.957, 0.643, 0.376};
      Color const Color::SeaGreen{0.180, 0.545, 0.341};
      Color const Color::Seashell{1.000, 0.961, 0.933};
      Color const Color::Sienna{0.627, 0.322, 0.176};
      Color const Color::Silver{0.753, 0.753, 0.753};
      Color const Color::SkyBlue{0.529, 0.808, 0.922};
      Color const Color::SlateBlue{0.416, 0.353, 0.804};
      Color const Color::SlateGray{0.439, 0.502, 0.565};
      Color const Color::Snow{1.000, 0.980, 0.980};
      Color const Color::SpringGreen{0.000, 1.000, 0.498};
      Color const Color::SteelBlue{0.275, 0.510, 0.706};
      Color const Color::Tan{0.824, 0.706, 0.549};
      Color const Color::Teal{0.000, 0.502, 0.502};
      Color const Color::Thistle{0.847, 0.749, 0.847};
      Color const Color::Tomato{1.000, 0.388, 0.278};
      Color const Color::Turquoise{0.251, 0.878, 0.816};
      Color const Color::Violet{0.933, 0.510, 0.933};
      Color const Color::Wheat{0.961, 0.871, 0.702};
      Color const Color::White{1.000, 1.000, 1.000};
      Color const Color::WhiteSmoke{0.961, 0.961, 0.961};
      Color const Color::Yellow{1.000, 1.000, 0.000};
      Color const Color::YellowGreen{0.604, 0.804, 0.196};
      Color const Color::Zero{0.0, 0.0, 0.0, 0.0};
      Color const Color::ZeroWhite{1.0, 1.0, 1.0, 0.0};
   }
}


// EOF
