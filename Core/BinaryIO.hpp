//-----------------------------------------------------------------------------
//
// Copyright (C) 2017-2019 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Binary data reading/writing primitives.
//
//-----------------------------------------------------------------------------

#ifndef DGE__Core__BinaryIO_H__
#define DGE__Core__BinaryIO_H__

#include "Types.hpp"

#include <array>
#include <climits>
#include <istream>
#include <ostream>


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Core
{
   std::uint_fast8_t  ReadLE1(char const *data);
   std::uint_fast16_t ReadLE2(char const *data);
   std::uint_fast32_t ReadLE4(char const *data);

   template<std::size_t N>
   std::array<char, N> ReadData(std::istream &in);

   template<typename T>
   T ReadVLN(std::istream &in);

   template<typename T>
   void WriteVLN(std::ostream &out, T in);

   //
   // ReadLE1
   //
   inline std::uint_fast8_t ReadLE1(char const *data)
   {
      return static_cast<std::uint_fast8_t>(static_cast<Byte>(data[0]));
   }

   //
   // ReadLE2
   //
   inline std::uint_fast16_t ReadLE2(char const *data)
   {
      return
         (static_cast<std::uint_fast16_t>(static_cast<Byte>(data[0])) << 0) |
         (static_cast<std::uint_fast16_t>(static_cast<Byte>(data[1])) << 8);
   }

   //
   // ReadLE4
   //
   inline std::uint_fast32_t ReadLE4(char const *data)
   {
      return
         (static_cast<std::uint_fast32_t>(static_cast<Byte>(data[0])) <<  0) |
         (static_cast<std::uint_fast32_t>(static_cast<Byte>(data[1])) <<  8) |
         (static_cast<std::uint_fast32_t>(static_cast<Byte>(data[2])) << 16) |
         (static_cast<std::uint_fast32_t>(static_cast<Byte>(data[3])) << 24);
   }

   //
   // ReadData
   //
   template<std::size_t N>
   std::array<char, N> ReadData(std::istream &in)
   {
      std::array<char, N> data;
      in.read(data.data(), N);
      return data;
   }

   //
   // ReadVLN
   //
   template<typename T>
   T ReadVLN(std::istream &in)
   {
      T out{0};

      unsigned char c;
      while(((c = in.get()) & 0x80) && in)
         out = (out << 7) + (c & 0x7F);
      out = (out << 7) + c;

      return out;
   }

   //
   // WriteVLN
   //
   template<typename T>
   void WriteVLN(std::ostream &out, T in)
   {
      constexpr std::size_t len = (sizeof(T) * CHAR_BIT + 6) / 7;
      char buf[len], *ptr = buf + len;

      *--ptr = static_cast<char>(in & 0x7F);
      while((in >>= 7))
         *--ptr = static_cast<char>(in & 0x7F) | 0x80;

      out.write(ptr, (buf + len) - ptr);
   }
}

#endif//DGE__Core__BinaryIO_H__

