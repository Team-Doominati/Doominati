//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Script VM tuple pointers.
//
//-----------------------------------------------------------------------------

#ifndef DGE__Code__TuplePtr_H__
#define DGE__Code__TuplePtr_H__

#include "../Code/MemPtr.hpp"

#include <tuple>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::Code
{
   //
   // MemInfo<std::tuple>
   //
   template<typename... Args>
   struct MemInfo<std::tuple<Args...> const>
   {
      static std::tuple<Args...> Get(Memory *mem, Word idx);

      static std::tuple<Args...> Get(Word const *mem);

      constexpr static unsigned int Shift = MemInfo<Word>::Shift;
      constexpr static unsigned int Step  = (MemInfo<Args>::Step + ...);
   };

   template<typename... Args>
   struct MemInfo<std::tuple<Args...>> : MemInfo<std::tuple<Args...> const>
   {
      static void Set(Memory *mem, Word idx, Args const &...val)
         {((MemInfo<Args>::Set(mem, idx, val), idx += MemInfo<Args>::Step), ...);}

      static void Set(Memory *mem, Word idx, std::tuple<Args...> const &val)
         {std::apply([&](Args const &...v){Set(mem, idx, v...);}, val);}

      static void Set(Word *mem, Args const &...val)
         {((MemInfo<Args>::Set(mem, val), mem += MemInfo<Args>::Step), ...);}

      static void Set(Word *mem, std::tuple<Args...> const &val)
         {std::apply([&](Args const &...v){Set(mem, v...);}, val);}
   };

   //
   // MemInfo<std::tuple<>>
   //
   template<>
   struct MemInfo<std::tuple<> const>
   {
      static std::tuple<> Get(Memory *, Word) {return {};}

      static std::tuple<> Get(Word const *) {return {};}

      constexpr static unsigned int Shift = MemInfo<Word>::Shift;
      constexpr static unsigned int Step  = 0;
   };

   template<>
   struct MemInfo<std::tuple<>> : MemInfo<std::tuple<> const>
   {
      static void Set(Memory *, Word) {}
      static void Set(Memory *, Word, std::tuple<> const &) {}

      static void Set(Word *) {}
      static void Set(Word *, std::tuple<> const &) {}
   };
}

#endif//DGE__Code__TuplePtr_H__

