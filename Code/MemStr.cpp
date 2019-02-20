//-----------------------------------------------------------------------------
//
// Copyright (C) 2016-2019 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Script VM memory strings utilities.
//
//-----------------------------------------------------------------------------

#include "Code/MemStr.hpp"


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Code
{
   //
   // MemMemCpy
   //
   char *MemMemCpy(char *out, MemPtr<Byte const> in, std::size_t len)
   {
      for(auto itr = out; len--;)
         *itr++ = *in++;

      return out;
   }

   //
   // MemStrNCpy
   //
   MemPtr<Byte> MemStrNCpy(MemPtr<Byte> out, Word outL, char const *in, std::size_t inL)
   {
      if(outL)
      {
         if(outL > inL)
            outL = inL + 1;

         auto itr = out;

         for(auto end = in + outL - 1; in != end;)
            *itr++ = *in++;

         *itr++ = '\0';
      }

      return out;
   }

   //
   // MemStrDup
   //
   std::unique_ptr<char[]> MemStrDup(MemPtr<Byte const> str)
   {
      return MemStrDup(str, MemStrLen(str));
   }

   //
   // MemStrDup
   //
   std::unique_ptr<char[]> MemStrDup(MemPtr<Byte const> str, std::size_t len)
   {
      std::unique_ptr<char[]> dup{new char[len+1]};

      MemMemCpy(dup.get(), str, len);
      dup[len] = '\0';

      return dup;
   }

   //
   // MemStrDupLen
   //
   std::pair<std::unique_ptr<char[]>, std::size_t> MemStrDupLen(MemPtr<Byte const> str)
   {
      std::size_t len = MemStrLen(str);
      return {MemStrDup(str, len), len};
   }

   //
   // MemStrLen
   //
   std::size_t MemStrLen(MemPtr<Byte const> str)
   {
      std::size_t len = 0;
      for(; *str; ++str) ++len;
      return len;
   }
}

// EOF

