//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Generic coordinate set handling.
//
//-----------------------------------------------------------------------------

#ifndef DGE__Core__Point_H__
#define DGE__Core__Point_H__


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace DGE::Core
{
   template<typename T, unsigned I>
   class Point;

   //
   // Point<T, 2>
   //
   template<typename T>
   class Point<T, 2>
   {
   public:
      T x, y;
   };

   //
   // Point<T, 3>
   //
   template<typename T>
   class Point<T, 3>
   {
   public:
      T x, y, z;
   };
}

#endif//DGE__Core__Point_H__

