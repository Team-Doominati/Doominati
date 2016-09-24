//-----------------------------------------------------------------------------
//
// Copyright (C) 2016 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// String utilities.
//
//-----------------------------------------------------------------------------

#ifndef DGE__Core__String_H__
#define DGE__Core__String_H__

#include <GDCC/Core/String.hpp>

#include <cstring>
#include <utility>
#include <tuple>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::Core
{
   //
   // HashedStr
   //
   class HashedStr
   {
   public:
      HashedStr() : str{nullptr}, len{0}, hash{0} {}
      HashedStr(char const *str_) :
         str{str_} {std::tie(len, hash) = GDCC::Core::StrLenHash(str);}
      HashedStr(char const *str_, std::size_t len_) :
         str{str_}, len{len_}, hash{GDCC::Core::StrHash(str, len)} {}
      HashedStr(char const *str_, std::size_t len_, std::size_t hash_) :
         str{str_}, len{len_}, hash{hash_} {}

      bool operator == (HashedStr const &r) const
         {return hash == r.hash && len == r.len && std::strcmp(str, r.str) == 0;}
      bool operator != (HashedStr const &r) const
         {return hash != r.hash || len != r.len || std::strcmp(str, r.str) != 0;}

      char const *str;
      std::size_t len;
      std::size_t hash;
   };
}

namespace std
{
   //
   // hash<::DGE::Core::HashedStr>
   //
   template<>
   struct hash<::DGE::Core::HashedStr>
   {
      constexpr size_t operator () (::DGE::Core::HashedStr const &str) const
         {return str.hash;}
   };
}

#endif//DGE__Core__String_H__

