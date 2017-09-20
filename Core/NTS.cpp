//-----------------------------------------------------------------------------
//
// Copyright (C) 2016-2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Null-Terminated String data handling.
//
//-----------------------------------------------------------------------------

#include "Core/NTS.hpp"

#include <GDCC/Core/Exception.hpp>
#include <GDCC/Core/Token.hpp>

#include <cstring>


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Core
{
   //
   // NTSArray constructor
   //
   NTSArray::NTSArray(char const *data, std::size_t dataLen) :
      str0{""}, str1{""}, str2{""}
   {
      char const *dataItr = data;
      char const *dataEnd = data + dataLen;
      std::size_t strn    = 0;

      // Count nulls in data block.
      while(dataItr != dataEnd)
         if(!*dataItr++) ++strn;

      // A meaningful NTS file must have at least three strings.
      if(strn < 3) return;

      dataItr = data;

      // Header strings.
      str0 = dataItr;
      str1 = dataItr += std::strlen(dataItr) + 1;
      str2 = dataItr += std::strlen(dataItr) + 1;

      // Body strings.
      for(auto &str : strs = {strn - 3, nullptr})
         str = dataItr += std::strlen(dataItr) + 1;
   }

   //
   // NTSStream::drop
   //
   bool NTSStream::drop(char const *tok)
   {
      return std::strcmp(tok, peek()) == 0 ? get(), true : false;
   }

   //
   // NTSStream::expect
   //
   void NTSStream::expect(char const *tok)
   {
      if(!drop(tok))
         throw GDCC::Core::ParseExceptExpect{tok, peek(), true};
   }

   //
   // NTSStream::peek
   //
   bool NTSStream::peek(char const *tok)
   {
      return std::strcmp(tok, peek()) == 0;
   }
}

// EOF

