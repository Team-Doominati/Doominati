//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
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

#include <climits>


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Core
{
   std::uint_fast8_t  ReadLE1(char const *data);
   std::uint_fast16_t ReadLE2(char const *data);
   std::uint_fast32_t ReadLE4(char const *data);

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
}

#endif//DGE__Core__BinaryIO_H__

