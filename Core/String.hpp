//-----------------------------------------------------------------------------
//
// Copyright (C) 2016-2017 Team Doominati
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
      HashedStr(char const *first, char const *last) :
         HashedStr{first, static_cast<std::size_t>(last - first)} {}
      HashedStr(char const *str_, std::size_t len_) :
         str{str_}, len{len_}, hash{GDCC::Core::StrHash(str, len)} {}
      HashedStr(char const *str_, std::size_t len_, std::size_t hash_) :
         str{str_}, len{len_}, hash{hash_} {}

      explicit operator bool () const {return str;}
      operator std::string () const {return std::string{str, len};}

      bool operator == (HashedStr const &r) const
         {return hash == r.hash && len == r.len && std::memcmp(str, r.str, len) == 0;}
      bool operator != (HashedStr const &r) const
         {return hash != r.hash || len != r.len || std::memcmp(str, r.str, len) != 0;}

      char const *begin() const {return str;}

      char const *data() const {return str;}

      bool empty() const {return !len;}

      char const *end() const {return str + len;}

      std::size_t size() const {return len;}

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


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace DGE::Core
{
   //
   // operator std::string == HashedStr
   //
   inline bool operator == (std::string const &l, HashedStr const &r)
   {
      return l.size() == r.size() && !std::memcmp(l.data(), r.data(), r.size());
   }

   //
   // operator HashedStr == std::string
   //
   inline bool operator == (HashedStr const &l, std::string const &r)
   {
      return l.size() == r.size() && !std::memcmp(l.data(), r.data(), l.size());
   }

   //
   // operator std::ostream << HashedStr
   //
   inline std::ostream &operator << (std::ostream &out, HashedStr const &in)
   {
      return out.write(in.str, in.len);
   }
}

#endif//DGE__Core__String_H__

