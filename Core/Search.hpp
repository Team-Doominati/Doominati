//-----------------------------------------------------------------------------
//
// Copyright (C) 2016 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Searching utilities.
//
//-----------------------------------------------------------------------------

#ifndef Doom__Core__Search_H__
#define Doom__Core__Search_H__

#include <algorithm>
#include <cstring>


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace Doom
{
   namespace Core
   {
      //
      // BinarySearch
      //
      template<typename T> T *BSearchStr(T *itr, T *end, char const *key)
      {
         auto cmpL = [](T const &i, char const *k) {return std::strcmp(i.name, k) < 0;};
         auto cmpU = [](char const *k, T const &i) {return std::strcmp(k, i.name) < 0;};

         itr = std::lower_bound(itr, end, key, cmpL);
         end = std::upper_bound(itr, end, key, cmpU);

         return itr == end ? nullptr : itr;
      }
   }
}

#endif//Doom__Core__Search_H__

