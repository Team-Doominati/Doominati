//-----------------------------------------------------------------------------
//
// Copyright (C) 2016 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Coordinates.
//
//-----------------------------------------------------------------------------

#ifndef Doom__Core__Core_H__
#define Doom__Core__Core_H__

#include <cstdint>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace Doom
{
   namespace Core
   {
      //
      // CoordDraw
      //
      class CoordDraw
      {
      public:
         using Ord = std::int_least16_t;


         CoordDraw() = default;
         CoordDraw(Ord x_, Ord y_) : x{x_}, y{y_} {}

         Ord x, y;
      };

      //
      // CoordTile
      //
      class CoordTile
      {
      public:
         using Ord = std::int_least16_t;


         CoordTile() = default;
         CoordTile(Ord x_, Ord y_) : x{x_}, y{y_} {}

         Ord x, y;
      };
   }
}

#endif//Doom__Core__Core_H__

