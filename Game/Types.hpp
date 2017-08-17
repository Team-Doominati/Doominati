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

#include "Core/Fixed.hpp"
#include "Core/Point.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::Game
{
   class Entity;
   class Object;
   class Team;
   class Thinker;

   using Angle = Core::Fixed<std::uint32_t, 32>;
   using Coord = Core::Fixed<std:: int32_t,  7>;

   using Point2 = Core::Point<Coord, 2>;
}

#endif//DGE__Game__Types_H__

