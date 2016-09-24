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

namespace DGE
{
   namespace Core
   {
      using Matrix4 = Eigen::Matrix4f;
   }
}


//----------------------------------------------------------------------------|
// Static Functions                                                           |
//

namespace DGE
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

namespace DGE
{
   namespace Core
   {
      constexpr float pi = 3.14159265359f;
      constexpr float pi2 = pi / 2.0f;
      constexpr float pi4 = pi / 4.0f;
      constexpr float tau = pi * 2.0f;
   }
}

#endif//DGE__Core__Math_H__

