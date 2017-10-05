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

#ifndef DGE__Core__Vector2_H__
#define DGE__Core__Vector2_H__

#include <cmath>
#include "Core/Math.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::Core
{
   //
   // Vector2T
   //
   template<typename T>
   class Vector2T
   {
   public:
      T x, y;

      Vector2T() = default;
      Vector2T(Vector2T &&) = default;
      Vector2T(Vector2T const &) = default;

      constexpr Vector2T(T x_, T y_) : x{x_}, y{y_} {}
      constexpr Vector2T<T> &operator=(Vector2T<T> const &o) = default;

      explicit operator bool() const {return x && y;}

      auto operator-() const {return Vector2T<T>{-x, -y};}
      bool operator!() const {return !(x && y);}

      auto operator+(Vector2T<T> const &o) const
         {return Vector2T<T>{x + o.x, y + o.y};}
      auto operator-(Vector2T<T> const &o) const
         {return Vector2T<T>{x - o.x, y - o.y};}

      auto &operator+=(Vector2T<T> const &o)
         {x += o.x; y += o.y; return *this;}
      auto &operator-=(Vector2T<T> const &o)
         {x -= o.x; y -= o.y; return *this;}
   };

   //
   // Vector2
   //
   class Vector2 : public Vector2T<float>
   {
   public:
      using Vector2T<float>::Vector2T;
      using Vector2T<float>::operator=;

      Vector2 &interpolate(Vector2 const &o, float amt)
      {
         x = Core::Lerp(x, o.x, amt);
         y = Core::Lerp(y, o.y, amt);
         return *this;
      }

      float length() const {return std::sqrt((x * x) + (y * y));}
      float dot(Vector2 const &o) const {return (x * o.x) + (y * o.y);}
   };

   //
   // Vector2I
   //
   class Vector2I : public Vector2T<int>
   {
   public:
      using Vector2T<int>::Vector2T;
      using Vector2T<int>::operator=;
   };
}

#endif//DGE__Core__Vector2_H__

