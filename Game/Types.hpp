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


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::Game
{
   using Angle = Core::Fixed<std::uint32_t, 32>;
   using Coord = Core::Fixed<std:: int32_t,  7>;
}

#endif//DGE__Game__Types_H__

