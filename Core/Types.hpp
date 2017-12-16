//-----------------------------------------------------------------------------
//
// Copyright (C) 2016-2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Common typedefs.
//
//-----------------------------------------------------------------------------

#ifndef DGE__Core__Types_H__
#define DGE__Core__Types_H__

#include <cstdint>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace std
{
   template<typename T>
   struct hash;
}

namespace DGE::Core
{
   using Byte = unsigned char;

   template<typename I, unsigned FB>
   class Fixed;
   template<typename Key, typename T, typename Hash = std::hash<Key>>
   class HashMapFixed;
   class HashedStr;
   class NTSArray;
   class NTSStream;
   template<typename T, unsigned I>
   class Point;
   template<typename ResourceData>
   class Resource;
}

#endif//DGE__Core__Types_H__

