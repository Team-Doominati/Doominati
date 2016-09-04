//-----------------------------------------------------------------------------
//
// Copyright (C) 2016 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Floating-point 2-dimensional vector.
//
//-----------------------------------------------------------------------------

#ifndef Doom__Core__Vector2_H__
#define Doom__Core__Vector2_H__

#include <cmath>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace Doom
{
   namespace Core
   {
      class Vector2
      {
      public:
         float x, y;
         
         //
         // Vector2::add
         //
         
         Vector2 &add(Vector2 const &other)
         {
            x += other.x;
            y += other.y;
            return *this;
         }
         
         //
         // Vector2::sub
         //
         
         Vector2 &sub(Vector2 const &other)
         {
            x -= other.x;
            y -= other.y;
            return *this;
         }
         
         //
         // Vector2::length
         //
         // Magnitude of vector.
         //
         
         float length()
         {
            return std::sqrt((x * x) + (y * y));
         }
         
         //
         // Vector2::dot
         //
         // Dot product of this and other.
         //
         
         float dot(Vector2 const &other)
         {
            return (x * other.x) + (y * other.y);
         }
         
         //
         // Vector2 Unary operators
         //
         
         Vector2 operator-()
         {
            return { -x, -y };
         }
         
         bool operator!()
         {
            return x && y;
         }
         
         //
         // Vector2 Infix operators
         //
         
         Vector2 operator+(Vector2 const &other)
         {
            return { x + other.x, y + other.y };
         }
         
         Vector2 operator-(Vector2 const &other)
         {
            return { x - other.x, y - other.y };
         }
         
         float operator*(Vector2 const &other)
         {
            return dot(other);
         }
         
         //
         // Vector2 Assignment operators
         //
         
         Vector2 &operator+=(Vector2 const &other)
         {
            return add(other);
         }
         
         Vector2 &operator-=(Vector2 const &other)
         {
            return sub(other);
         }
      };
   }
}

#endif//Doom__Core__Vector2_H__

