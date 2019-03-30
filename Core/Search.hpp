//-----------------------------------------------------------------------------
//
// Copyright (C) 2016-2019 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Searching utilities.
//
//-----------------------------------------------------------------------------

#ifndef DGE__Core__Search_H__
#define DGE__Core__Search_H__

#include <algorithm>
#include <cstring>


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Core
{
   //
   // BSearchMem
   //
   template<typename Itr, typename Key, typename KeyGet>
   Itr BSearchMem(Itr itr, Itr end, Key key, KeyGet keyGet)
   {
      using T = typename std::iterator_traits<Itr>::value_type;

      auto cmpL = [&keyGet](T const &i, Key const &k) {return keyGet(i) < k;};
      auto cmpU = [&keyGet](Key const &k, T const &i) {return k < keyGet(i);};

      itr = std::lower_bound(itr, end, key, cmpL);
      end = std::upper_bound(itr, end, key, cmpU);

      return itr;
   }

   //
   // BSearchMemPtr
   //
   template<typename Itr, typename Key, typename KeyGet>
   typename std::iterator_traits<Itr>::pointer
   BSearchMemPtr(Itr itr, Itr end, Key key, KeyGet keyGet)
   {
      itr = BSearchMem(itr, end, std::move(key), std::move(keyGet));
      return itr == end ? nullptr : &*itr;
   }
}

#endif//DGE__Core__Search_H__

