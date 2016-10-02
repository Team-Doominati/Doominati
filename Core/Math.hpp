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
   // Lerp
   //
   static constexpr float Lerp(float x, float y, float amt)
   {
      return ((1.0f - amt) * x) + (amt * y);
   }
}


//----------------------------------------------------------------------------|
// Static Objects                                                             |
//

namespace DGE::Core
{
   static constexpr float Pi = 3.14159265359f;
   static constexpr float Pi2 = Pi / 2.0f;
   static constexpr float Pi4 = Pi / 4.0f;
   static constexpr float Tau = Pi * 2.0f;
}

#endif//DGE__Core__Math_H__

