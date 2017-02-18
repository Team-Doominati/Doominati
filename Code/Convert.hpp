//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Script data to/from host data conversion.
//
//-----------------------------------------------------------------------------

#ifndef DGE__Code__Convert_H__
#define DGE__Code__Convert_H__

#include "Code/Types.hpp"

#include <cmath>


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Code
{
   //
   // HostToSAccum
   //
   constexpr Word HostToSAccum(std::double_t x)
   {
      return static_cast<SWord>(x * 128);
   }

   //
   // HostToULFract
   //
   constexpr Word HostToULFract(std::double_t x)
   {
      return static_cast<Word>(x * 0xFFFFFFFF);
   }

   //
   // SAccumToHost
   //
   constexpr std::double_t SAccumToHost(Word x)
   {
      return static_cast<SWord>(x) / 128.0;
   }

   //
   // SFractToHost
   //
   constexpr std::float_t SFractToHost(Word x)
   {
      return x & 0x8000
         ? -static_cast<SWord>(x & 0xFFFF) / static_cast<std::float_t>(0x8000)
         :  static_cast<SWord>(x & 0xFFFF) / static_cast<std::float_t>(0x7FFF);
   }

   //
   // SFractToSLFract
   //
   constexpr Word SFractToSLFract(Word x)
   {
      return (x << 16) | ((x & 0x7FFF) << 1);
   }

   //
   // SLFractToHost
   //
   constexpr std::double_t SLFractToHost(Word x)
   {
      return x & 0x80000000
         ? static_cast<SWord>(x) / static_cast<std::double_t>(0x80000000)
         : static_cast<SWord>(x) / static_cast<std::double_t>(0x7FFFFFFF);
   }

   //
   // ULFractToHost
   //
   constexpr std::double_t ULFractToHost(Word x)
   {
      return x / static_cast<std::double_t>(0xFFFFFFFF);
   }

   //
   // WordToDWord
   //
   constexpr DWord WordToDWord(Word x0, Word x1)
      {return x0 | (static_cast<DWord>(x1) << 32);}
   constexpr DWord WordToDWord(Word x[2])
      {return x[0] | (static_cast<DWord>(x[1]) << 32);}
}

#endif//DGE__Code__Convert_H__

