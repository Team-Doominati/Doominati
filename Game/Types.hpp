//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Common typedefs.
//
//-----------------------------------------------------------------------------

#ifndef DGE__Game__Types_H__
#define DGE__Game__Types_H__

#include "../Code/Types.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::Game
{
   class Entity;
   class Inventory;
   class ItemData;
   class ItemType;
   class Object;
   class Team;
   class Thinker;

   using Accum = Core::Fixed<std::uint32_t,  7>;
   using Angle = Core::Fixed<std::uint32_t, 32>;
   using Fixed = Core::Fixed<std:: int32_t,  7>;
   using Fract = Core::Fixed<std:: int32_t, 31>;

   using Point2X = Core::Point<Fixed, 2>;
}

#endif//DGE__Game__Types_H__

