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
#include <cstdlib>
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
   template<typename I_, unsigned FB>
   class Fixed
   {
   public:
      using I    = I_;
      using Eval = typename FixedEvalT<I>::Eval;
      using F    = typename FixedFloatT<I>::FloatT;


      Fixed() = default;
      constexpr Fixed(Fixed const &) = default;
      constexpr Fixed(I i) : data{i * FU} {}
      constexpr Fixed(I i, FixedRawT) : data{i} {}
      constexpr explicit Fixed(F f) : data{f * FUL} {}
      constexpr Fixed(typename FixedIntT<I>::Int i) : Fixed{static_cast<I>(i)} {}

      constexpr explicit operator bool () const {return static_cast<bool>(data);}
      constexpr explicit operator I () const {return data / FU;}
      constexpr explicit operator F () const {return static_cast<F>(data) / FUL;}

      constexpr Fixed operator + () const {return *this;}
      constexpr Fixed operator - () const {return {-data, FixedRawT{}};}
      constexpr Fixed operator ~ () const {return {~data, FixedRawT{}};}
      constexpr Fixed &operator ++ () {return data += FU, *this;}
      constexpr Fixed  operator ++ (int) {auto x = *this; return data += FU, x;}
      constexpr Fixed &operator -- () {return data -= FU, *this;}
      constexpr Fixed  operator -- (int) {auto x = *this; return data += FU, x;}

      constexpr Fixed operator * (Fixed const &r) const
         {return {Mul(data, r.data), FixedRawT{}};}
      constexpr Fixed operator * (I const &r) const
         {return {data * r, FixedRawT{}};}
      constexpr Fixed operator / (Fixed const &r) const
         {return {Div(data, r.data), FixedRawT{}};}
      constexpr Fixed operator / (I const &r) const
         {return {data / r, FixedRawT{}};}
      constexpr Fixed operator % (Fixed const &r) const
         {return {data % r.data, FixedRawT{}};}

      constexpr Fixed operator + (Fixed const &r) const
         {return {data + r.data, FixedRawT{}};}
      constexpr Fixed operator - (Fixed const &r) const
         {return {data - r.data, FixedRawT{}};}

      constexpr Fixed operator << (unsigned r) const {return {data << r, FixedRawT{}};}
      constexpr Fixed operator >> (unsigned r) const {return {data >> r, FixedRawT{}};}

      constexpr bool operator <  (Fixed const &r) const {return data <  r.data;}
      constexpr bool operator >  (Fixed const &r) const {return data >  r.data;}
      constexpr bool operator <= (Fixed const &r) const {return data <= r.data;}
      constexpr bool operator >= (Fixed const &r) const {return data >= r.data;}

      constexpr bool operator == (Fixed const &r) const {return data == r.data;}
      constexpr bool operator != (Fixed const &r) const {return data != r.data;}

      constexpr Fixed &operator *= (Fixed const &r) {return data = Mul(data, r.data), *this;}
      constexpr Fixed &operator /= (Fixed const &r) {return data = Div(data, r.data), *this;}
      constexpr Fixed &operator %= (Fixed const &r) {return data %= r.data, *this;}
      constexpr Fixed &operator += (Fixed const &r) {return data += r.data, *this;}
      constexpr Fixed &operator -= (Fixed const &r) {return data -= r.data, *this;}
      constexpr Fixed &operator <<= (unsigned r) {return data << r, *this;}
      constexpr Fixed &operator >>= (unsigned r) {return data >> r, *this;}

      constexpr I raw() const {return data;}
      Fixed &raw(I i) {return data = i, *this;}

      constexpr F toF() const {return static_cast<F>(data) / FUL;}
      constexpr I toI() const {return data / FU;}


      static constexpr I Div(I l, I r) {return (static_cast<Eval>(l) << FB) / r;}
      static constexpr I Mul(I l, I r) {return (static_cast<Eval>(l) * r) >> FB;}
      static constexpr Fixed Raw(I i) {return {i, FixedRawT{}};}

      static constexpr Eval FUL = static_cast<Eval>(1) << FB;
      static constexpr I    FU  = static_cast<I>(FUL);
      static constexpr I    FM  = FU - 1;

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


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Core
{
   //
   // operator Fixed - Fixed::F
   //
   template<typename I, unsigned FB>
   constexpr typename Fixed<I, FB>::F operator -
      (Fixed<I, FB> const &l, typename Fixed<I, FB>::F const &r)
   {
      return static_cast<typename Fixed<I, FB>::F>(l) - r;
   }

   //
   // operator Fixed::F - Fixed
   //
   template<typename I, unsigned FB>
   constexpr typename Fixed<I, FB>::F operator -
      (typename Fixed<I, FB>::F const &l, Fixed<I, FB> const &r)
   {
      return l - static_cast<typename Fixed<I, FB>::F>(r);
   }
}

namespace std
{
   //
   // abs
   //
   template<typename I, unsigned FB>
   ::DGE::Core::Fixed<I, FB> abs(::DGE::Core::Fixed<I, FB> const &x)
   {
      return x < ::DGE::Core::Fixed<I, FB>{0} ? -x : x;
   }

   //
   // ceil
   //
   template<typename I, unsigned FB>
   ::DGE::Core::Fixed<I, FB> ceil(::DGE::Core::Fixed<I, FB> const &x)
   {
      I i = x.raw();

      if(i & x.FM)
      {
         if(i < 0)
            i = (i & ~x.FM) - x.FU;
         else
            i = (i & ~x.FM) + x.FU;
      }

      return {i, ::DGE::Core::FixedRaw};
   }
}

#endif//DGE__Core__Fixed_H__

