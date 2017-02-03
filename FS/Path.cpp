//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Virtual filesystem and archive path handling.
//
//-----------------------------------------------------------------------------

#include "FS/Path.hpp"


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::FS
{
   //
   // PathCompare
   //
   int PathCompare(char const *l, char const *r)
   {
      return PathCompare(l, std::strlen(l), r, std::strlen(r));
   }

   //
   // PathCompare
   //
   int PathCompare(char const *l, std::size_t lenL, char const *r, std::size_t lenR)
   {
      char const *ls = static_cast<char const *>(std::memchr(l, '/', lenL));
      char const *rs = static_cast<char const *>(std::memchr(r, '/', lenR));

      // Both are final component.
      if(!ls && !rs)
      {
         if(auto res = std::memcmp(l, r, std::min(lenL, lenR)))
            return res;

         if(lenL == lenR) return 0;
         return lenL < lenR ? -1 : +1;
      }

      // Left is final component, right is not.
      if(!ls) return -1;

      // Right is final component, left is not.
      if(!rs) return +1;

      // If the bases are inequal, that determines sorting.
      for(auto li = l, ri = r;; ++li, ++ri)
      {
         if(*li == '/')
         {
            if(*ri == '/') break;
            return -1;
         }

         if(*ri == '/')
            return +1;

         if(*li != *ri)
            return *li - *ri;
      }

      // Otherwise, compare after the bases.
      return PathCompare(ls + 1, l + lenL - ls - 1, rs + 1, r + lenR - rs - 1);
   }
}

// EOF

