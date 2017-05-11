//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Script VM memory pointers for Game types.
//
//-----------------------------------------------------------------------------

#ifndef DGE__Game__MemFuncs_H__
#define DGE__Game__MemFuncs_H__

#include "Game/Types.hpp"

#include "Code/MemPtr.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::Code
{
   //
   // MemInfo<Game::Point2>
   //
   template<>
   struct MemInfo<Game::Point2 const>
   {
      static Game::Point2 Get(Memory *mem, Word idx)
      {
         return {{static_cast<SWord>(mem->getW(idx)), Core::FixedRaw},
            {static_cast<SWord>(mem->getW(idx + MemInfo<Word>::Step)), Core::FixedRaw}};
      }

      constexpr static unsigned int Shift = MemInfo<Word>::Shift;
      constexpr static unsigned int Step  = MemInfo<Word>::Step * 2;
   };
   template<>
   struct MemInfo<Game::Point2> : MemInfo<Game::Point2 const>
   {
      static void Set(Memory *mem, Word idx, Game::Point2 val)
      {
         mem->setW(idx, val.x.raw());
         mem->setW(idx + MemInfo<Word>::Step, val.y.raw());
      }
   };
}

#endif//DGE__Game__MemFuncs_H__

