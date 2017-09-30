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
   // MemInfo<Game::Coord>
   //
   template<>
   struct MemInfo<Game::Coord const>
   {
      static Game::Coord Get(Memory *mem, Word idx)
      {
         return Game::Coord::Raw(mem->getW(idx));
      }

      constexpr static unsigned int Shift = MemInfo<Word>::Shift;
      constexpr static unsigned int Step  = MemInfo<Word>::Step;
   };
   template<>
   struct MemInfo<Game::Coord> : MemInfo<Game::Coord const>
   {
      static void Set(Memory *mem, Word idx, Game::Coord val)
      {
         mem->setW(idx, val.raw());
      }
   };

   //
   // MemInfo<Game::Fract>
   //
   template<>
   struct MemInfo<Game::Fract const>
   {
      static Game::Fract Get(Memory *mem, Word idx)
      {
         return Game::Fract::Raw(mem->getW(idx));
      }

      constexpr static unsigned int Shift = MemInfo<Word>::Shift;
      constexpr static unsigned int Step  = MemInfo<Word>::Step;
   };
   template<>
   struct MemInfo<Game::Fract> : MemInfo<Game::Fract const>
   {
      static void Set(Memory *mem, Word idx, Game::Fract val)
      {
         mem->setW(idx, val.raw());
      }
   };

   //
   // MemInfo<Game::Point2>
   //
   template<>
   struct MemInfo<Game::Point2 const>
   {
      static Game::Point2 Get(Memory *mem, Word idx)
      {
         return
         {
            Game::Coord::Raw(mem->getW(idx)),
            Game::Coord::Raw(mem->getW(idx + MemInfo<Word>::Step))
         };
      }

      constexpr static unsigned int Shift = MemInfo<Word>::Shift;
      constexpr static unsigned int Step  = MemInfo<Word>::Step * 2;
   };
   template<>
   struct MemInfo<Game::Point2> : MemInfo<Game::Point2 const>
   {
      static void Set(Memory *mem, Word idx, Game::Point2 val)
      {
         mem->setW(idx,                       val.x.raw());
         mem->setW(idx + MemInfo<Word>::Step, val.y.raw());
      }
   };
}

#endif//DGE__Game__MemFuncs_H__

