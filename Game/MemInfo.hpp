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
   // MemInfo<Game::Fixed>
   //
   template<>
   struct MemInfo<Game::Fixed const>
   {
      static Game::Fixed Get(Memory *mem, Word idx)
      {
         return Game::Fixed::Raw(mem->getW(idx));
      }

      constexpr static unsigned int Shift = MemInfo<Word>::Shift;
      constexpr static unsigned int Step  = MemInfo<Word>::Step;
   };
   template<>
   struct MemInfo<Game::Fixed> : MemInfo<Game::Fixed const>
   {
      static void Set(Memory *mem, Word idx, Game::Fixed val)
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
   // MemInfo<Game::Point2X>
   //
   template<>
   struct MemInfo<Game::Point2X const>
   {
      static Game::Point2X Get(Memory *mem, Word idx)
      {
         return
         {
            Game::Fixed::Raw(mem->getW(idx)),
            Game::Fixed::Raw(mem->getW(idx + MemInfo<Word>::Step))
         };
      }

      constexpr static unsigned int Shift = MemInfo<Word>::Shift;
      constexpr static unsigned int Step  = MemInfo<Word>::Step * 2;
   };
   template<>
   struct MemInfo<Game::Point2X> : MemInfo<Game::Point2X const>
   {
      static void Set(Memory *mem, Word idx, Game::Point2X val)
      {
         mem->setW(idx,                       val.x.raw());
         mem->setW(idx + MemInfo<Word>::Step, val.y.raw());
      }
   };
}

#endif//DGE__Game__MemFuncs_H__

