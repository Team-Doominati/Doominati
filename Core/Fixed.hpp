//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Fixed-point value handling
//
//-----------------------------------------------------------------------------

#ifndef DGE__Core__Fixed_H__
#define DGE__Core__Fixed_H__

#include <cstdint>
#include <type_traits>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::Core
{
   class FixedRawT {};

   //
   // FixedEvalT
   //
   template<typename T>
   class FixedEvalT;

   template<> class FixedEvalT<std:: int32_t> {public: using Eval = std:: int64_t;};
   template<> class FixedEvalT<std::uint32_t> {public: using Eval = std::uint64_t;};

   //
   // FixedFloatT
   //
   template<typename T>
   class FixedFloatT {public: using FloatT = double;};

   //
   // FixedIntT
   //
   // Used when adding an int overload could cause duplicate overload.
   //
   template<typename T>
   class FixedIntT {public: using Int = int;};
   template<> class FixedIntT<int> {public: using Int = short;};

   //
   // Fixed
   //
   template<typename I, unsigned FB>
   class Fixed
   {
   public:
      using Eval = typename FixedEvalT<I>::Eval;
      using F    = typename FixedFloatT<I>::FloatT;


      Fixed() = default;
      constexpr Fixed(Fixed const &) = default;
      constexpr Fixed(I i) : data{i * FU} {}
      constexpr Fixed(I i, FixedRawT) : data{i} {}
      constexpr Fixed(F f) : data{f * FUL} {}
      constexpr Fixed(typename FixedIntT<I>::Int i) : Fixed{static_cast<I>(i)} {}

      constexpr operator I () const {return data / FU;}
      constexpr operator F () const {return static_cast<F>(data) / FUL;}

      constexpr Fixed operator + () const {return *this;}
      constexpr Fixed operator - () const {return {-data, FixedRawT{}};}
      constexpr Fixed operator ~ () const {return {~data, FixedRawT{}};}
      constexpr Fixed &operator ++ () {return data += FU, *this;}
      constexpr Fixed  operator ++ (int) {auto x = *this; return data += FU, x;}
      constexpr Fixed &operator -- () {return data -= FU, *this;}
      constexpr Fixed  operator -- (int) {auto x = *this; return data += FU, x;}

      constexpr Fixed operator * (Fixed const &r) const
         {return {Mul(data, r.data), FixedRawT{}};}
      constexpr Fixed operator / (Fixed const &r) const
         {return {Div(data, r.data), FixedRawT{}};}
      constexpr Fixed operator % (Fixed const &r) const
         {return {data % r.data, FixedRawT{}};}

      constexpr Fixed operator + (Fixed const &r) const
         {return {data + r.data, FixedRawT{}};}
      constexpr Fixed operator - (Fixed const &r) const
         {return {data - r.data, FixedRawT{}};}

      constexpr Fixed operator << (unsigned r) const {return {data << r, FixedRawT{}};}
      constexpr Fixed operator >> (unsigned r) const {return {data >> r, FixedRawT{}};}

      constexpr Fixed &operator *= (Fixed const &r) {return data = Mul(data, r.data), *this;}
      constexpr Fixed &operator /= (Fixed const &r) {return data = Div(data, r.data), *this;}
      constexpr Fixed &operator %= (Fixed const &r) {return data %= r.data, *this;}
      constexpr Fixed &operator += (Fixed const &r) {return data += r.data, *this;}
      constexpr Fixed &operator -= (Fixed const &r) {return data -= r.data, *this;}
      constexpr Fixed &operator <<= (unsigned r) {return data << r, *this;}
      constexpr Fixed &operator >>= (unsigned r) {return data >> r, *this;}

      constexpr I raw() const {return data;}
      Fixed &raw(I i) {return data = i, *this;}


      static constexpr I Div(I l, I r) {return (static_cast<Eval>(l) << FB) / r;}
      static constexpr I Mul(I l, I r) {return (static_cast<Eval>(l) * r) >> FB;}
      static constexpr Fixed Raw(I i) {return {i, FixedRawT{}};}

      static constexpr Eval FUL = static_cast<Eval>(1) << FB;
      static constexpr I    FU  = static_cast<I>(FUL);

   private:
      I data;
   };
}


//----------------------------------------------------------------------------|
// Extern Objects                                                             |
//

namespace DGE::Core
{
   constexpr FixedRawT FixedRaw;
}

#endif//DGE__Core__Fixed_H__

