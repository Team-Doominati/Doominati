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

#include "../Core/Types.hpp"

#include <cstdlib>
#include <limits>
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

      template<unsigned FBl, unsigned FBr>
      static constexpr I MulFB(I const &l, I const &r)
      {
         return (static_cast<Eval>(l) * r) >> (FBl + FBr - FB);
      }

      template<unsigned FBl, unsigned FBr>
      static constexpr Fixed MulFB(Fixed<I, FBl> const &l, Fixed<I, FBr> const &r)
      {
         return Raw(MulFB<FBl, FBr>(l.raw(), r.raw()));
      }

      static constexpr Eval FUL = static_cast<Eval>(1) << FB;
      static constexpr I    FU  = static_cast<I>(FUL);
      static constexpr I    FM  = FU - 1;

   private:
      I data;
   };
}

namespace std
{
   //
   // numeric_limits<::DGE::Core::Fixed>
   //
   template<typename I, unsigned FB>
   class numeric_limits<::DGE::Core::Fixed<I, FB>>
   {
   public:
      static constexpr ::DGE::Core::Fixed<I, FB> denorm_min()
         {return {1, ::DGE::Core::FixedRawT{}};}

      static constexpr ::DGE::Core::Fixed<I, FB> epsilon()
         {return {1, ::DGE::Core::FixedRawT{}};}

      static constexpr ::DGE::Core::Fixed<I, FB> infinity()
         {return {0, ::DGE::Core::FixedRawT{}};}

      static constexpr ::DGE::Core::Fixed<I, FB> lowest()
         {return {numeric_limits<I>::lowest(), ::DGE::Core::FixedRawT{}};}

      static constexpr ::DGE::Core::Fixed<I, FB> max()
         {return {numeric_limits<I>::max(), ::DGE::Core::FixedRawT{}};}

      static constexpr ::DGE::Core::Fixed<I, FB> min()
         {return {1, ::DGE::Core::FixedRawT{}};}

      static constexpr ::DGE::Core::Fixed<I, FB> quiet_NaN()
         {return {0, ::DGE::Core::FixedRawT{}};}

      static constexpr ::DGE::Core::Fixed<I, FB> round_error()
         {return {0, ::DGE::Core::FixedRawT{}};} // TODO

      static constexpr ::DGE::Core::Fixed<I, FB> signaling_NaN()
         {return {0, ::DGE::Core::FixedRawT{}};}

      static constexpr int digits       = numeric_limits<I>::digits;
      static constexpr int digits10     = numeric_limits<I>::digits10; // FIXME
      static constexpr int max_digits10 = 0; // TODO

      static constexpr float_denorm_style has_denorm        = denorm_absent;
      static constexpr bool               has_denorm_loss   = false;
      static constexpr bool               has_infinity      = false;
      static constexpr bool               has_quiet_NaN     = false;
      static constexpr bool               has_signaling_NaN = false;

      static constexpr bool is_bounded     = numeric_limits<I>::is_bounded;
      static constexpr bool is_exact       = false;
      static constexpr bool is_iec559      = false;
      static constexpr bool is_integer     = false;
      static constexpr bool is_modulo      = numeric_limits<I>::is_modulo;
      static constexpr bool is_signed      = numeric_limits<I>::is_signed;
      static constexpr bool is_specialized = true;

      static constexpr int max_exponent   = 0; // TODO
      static constexpr int max_exponent10 = 0; // TODO
      static constexpr int min_exponent   = 0; // TODO
      static constexpr int min_exponent10 = 0; // TODO

      static constexpr int radix = 2;

      static constexpr float_round_style round_style = round_toward_zero;

      static constexpr bool tinyness_before = numeric_limits<I>::tinyness_before;

      static constexpr bool traps = numeric_limits<I>::traps;
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

