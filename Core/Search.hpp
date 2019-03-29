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
   // BSearchKey
   //
   template<typename T, typename Key>
   T *BSearchKey(T *itr, T *end, Key key)
   {
      auto cmpL = [](T const &i, Key const &k) {return i.name < k;};
      auto cmpU = [](Key const &k, T const &i) {return k < i.name;};

      itr = std::lower_bound(itr, end, key, cmpL);
      end = std::upper_bound(itr, end, key, cmpU);

      return itr == end ? nullptr : itr;
   }

   //
   // BSearchKey
   //
   template<typename Itr, typename Key, typename KeyGet>
   typename std::iterator_traits<Itr>::pointer
   BSearchKey(Itr itr, Itr end, Key key, KeyGet keyGet)
   {
      using T = typename std::iterator_traits<Itr>::value_type;

      auto cmpL = [&keyGet](T const &i, Key const &k) {return keyGet(i) < k;};
      auto cmpU = [&keyGet](Key const &k, T const &i) {return k < keyGet(i);};

      itr = std::lower_bound(itr, end, key, cmpL);
      end = std::upper_bound(itr, end, key, cmpU);

      return itr == end ? nullptr : &*itr;
   }

   //
   // BSearchStr
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

#endif//DGE__Core__Search_H__

