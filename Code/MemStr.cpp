//-----------------------------------------------------------------------------
//
// Copyright (C) 2016 Team Doominati
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
   // MemStrNCpy
   //
   void MemStrNCpy(MemPtr<Byte> out, Word outL, char const *in, std::size_t inL)
   {
      if(outL)
      {
         if(outL > inL)
            outL = inL + 1;

         for(auto end = in + outL - 1; in != end;)
            *out++ = *in++;

         *out++ = '\0';
      }
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

      for(char *itr = dup.get(), *end = itr + len; itr != end; ++itr, ++str)
         *itr = *str;
      dup[len] = '\0';

      return dup;
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

