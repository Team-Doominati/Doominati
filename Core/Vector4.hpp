//-----------------------------------------------------------------------------
//
// Copyright (C) 2016 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Floating-point 4-dimensional vector.
//
//-----------------------------------------------------------------------------

#ifndef Doom__Core__Vector4_H__
#define Doom__Core__Vector4_H__

#include <cmath>
#include "Core/Math.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace Doom
{
   namespace Core
   {
      class Vector4
      {
      public:
         float x, y, z, w;
         
         //
         // Vector4::add
         //
         
         Vector4 &add(Vector4 const &other)
         {
            x += other.x;
            y += other.y;
            z += other.z;
            w += other.w;
            
            return *this;
         }
         
         //
         // Vector4::sub
         //
         
         Vector4 &sub(Vector4 const &other)
         {
            x -= other.x;
            y -= other.y;
            z -= other.z;
            w -= other.w;
            
            return *this;
         }
         
         //
         // Vector4::length
         //
         // Magnitude of vector.
         //
         
         float length() const
         {
            return std::sqrt((x * x) + (y * y) + (z * z) + (w * w));
         }
         
         //
         // Vector4::dot
         //
         // Dot product of this and other.
         //
         
         float dot(Vector4 const &other) const
         {
            return (x * other.x) + (y * other.y) + (z * other.z) + (w * other.w);
         }

         //
         // interpolate
         //

         Vector4 &interpolate(Vector4 const &other, float amt)
         {
            x = Core::Lerp(x, other.x, amt);
            y = Core::Lerp(y, other.y, amt);
            z = Core::Lerp(z, other.z, amt);
            w = Core::Lerp(w, other.w, amt);

            return *this;
         }
         
         //
         // Vector4 Unary operators
         //
         
         Vector4 operator-() const { return { -x, -y, -z, -w }; }
         bool operator!() const { return !(x && y && z && w); }
         
         //
         // Vector4 Conversion operators
         //
         
         explicit operator bool() const { return x && y && z && w; }
         
         //
         // Vector4 Infix operators
         //
         
         Vector4 operator+(Vector4 const &other) const
         {
            return { x + other.x, y + other.y, z + other.z, w + other.w };
         }
         
         Vector4 operator-(Vector4 const &other) const
         {
            return { x - other.x, y - other.y, z - other.z, w - other.w };
         }
         
         float operator*(Vector4 const &other) const
         {
            return dot(other);
         }
         
         //
         // Vector4 Assignment operators
         //
         
         Vector4 &operator+=(Vector4 const &other) { return add(other); }
         Vector4 &operator-=(Vector4 const &other) { return sub(other); }
      };
   }
}

#endif//Doom__Core__Vector4_H__

