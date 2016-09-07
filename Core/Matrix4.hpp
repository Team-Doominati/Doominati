//-----------------------------------------------------------------------------
//
// Copyright (C) 2016 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Floating-point 4x4 matrix.
//
//-----------------------------------------------------------------------------

#ifndef Doom__Core__Matrix4_H__
#define Doom__Core__Matrix4_H__

#include "Core/Vector4.hpp"

#include <cmath>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace Doom
{
   namespace Core
   {
      class Matrix4
      {
      public:
         Vector4 c0{1.0f, 0.0f, 0.0f, 0.0f};
         Vector4 c1{0.0f, 1.0f, 0.0f, 0.0f};
         Vector4 c2{0.0f, 0.0f, 1.0f, 0.0f};
         Vector4 c3{0.0f, 0.0f, 0.0f, 1.0f};
         
         float *getPointer() { return &c0.x; }
         float const *getConstPointer() const { return &c0.x; }
         
         //
         // Matrix4::identity
         //
         
         Matrix4 &identity()
         {
            c0 = Vector4{1.0f, 0.0f, 0.0f, 0.0f};
            c1 = Vector4{0.0f, 1.0f, 0.0f, 0.0f};
            c2 = Vector4{0.0f, 0.0f, 1.0f, 0.0f};
            c3 = Vector4{0.0f, 0.0f, 0.0f, 1.0f};
            
            return *this;
         }
         
         //
         // Matrix4::translate
         //
         
         Matrix4 &translate(float x, float y, float z = 0.0f)
         {
            c3.x = x;
            c3.y = y;
            c3.z = z;
            
            return *this;
         }
         
         //
         // Matrix4::rotate2d
         //
         
         Matrix4 &rotate2d(float ang)
         {
            c0.x =  std::cos(ang);
            c0.y =  std::sin(ang);
            c1.x = -std::sin(ang);
            c1.y =  std::cos(ang);
            
            return *this;
         }
      };
   }
}

#endif//Doom__Core__Matrix4_H__

