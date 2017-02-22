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

#ifndef DGE__Core__Math_H__
#define DGE__Core__Math_H__

#include <Eigen/Core>
#include <Eigen/Geometry>

#include <algorithm>


//----------------------------------------------------------------------------|
// Type Definitions                                                           |
//

namespace DGE::Core
{
   using Matrix4 = Eigen::Matrix4f;
}


//----------------------------------------------------------------------------|
// Static Functions                                                           |
//

namespace DGE::Core
{
   //
   // Clamp
   //
   // STDC_TODO: replace this with std::clamp when GCC 7 is mainline
   //
   template<class T, class Compare>
   constexpr T const &Clamp(T const &v, T const &lo, T const &hi, Compare comp)
   {
      return comp(v, lo) ? lo : comp(hi, v) ? hi : v;
   }

   //
   // Clamp
   //
   template<class T>
   constexpr T const &Clamp(T const &v, T const &lo, T const &hi)
   {
      return Clamp(v, lo, hi, std::less<>());
   }

   //
   // Lerp
   //
   constexpr float Lerp(float x, float y, float amt)
   {
      return ((1.0f - amt) * x) + (amt * y);
   }
}


//----------------------------------------------------------------------------|
// Static Objects                                                             |
//

namespace DGE::Core
{
   constexpr float Pi = 3.14159265359f;
   constexpr float Pi2 = Pi / 2.0f;
   constexpr float Pi4 = Pi / 4.0f;
   constexpr float Tau = Pi * 2.0f;
}

#endif//DGE__Core__Math_H__

